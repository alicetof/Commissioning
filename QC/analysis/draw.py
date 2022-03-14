#!/usr/bin/env python3


from shutil import ExecError
from utilities.plotting import draw_nice_canvas
from common import warning_msg, get_default_parser
from ROOT import TFile, TH1, TLatex
from os import path
import glob
import configparser


labels_drawn = []


def draw_label(label, x=0.55, y=0.96):
    label = label.strip()
    l = TLatex(x, y, label)
    l.SetNDC()
    l.Draw()
    l.SetTextAlign(21)
    l.SetTextFont(42)
    labels_drawn.append(l)
    return l


object_drawn = {}


def draw(filename,
         out_path="/tmp/",
         extension="png",
         configuration=None):
    filename = path.normpath(filename)
    f_tag = filename.split("/")[-2]
    can = draw_nice_canvas(f_tag)
    f = TFile(filename, "READ")
    h = None
    try:
        h = f.Get("ccdb_object")
    except:
        pass
    h.SetDirectory(0)
    h.SetName(h.GetTitle())
    object_drawn.append(h)
    drawopt = ""
    show_title = False
    h.SetBit(TH1.kNoTitle)
    if configuration is not None:
        if f_tag in configuration.sections():
            def get_option(opt, forcetype=None):
                src = "DEFAULT"
                if configuration.has_option(f_tag, opt):
                    src = f_tag
                o = configuration.get(src, opt)
                if forcetype is not None:
                    if forcetype == bool:
                        o = configuration.getboolean(src, opt)
                    else:
                        o = forcetype(o)
                print(o, type(o))
                return o

            drawopt = get_option("drawopt")
            show_title = get_option("showtitle", bool)
            if not get_option("showstats", bool):
                h.SetBit(TH1.kNoStats)

            def set_if_not_empty(opt, setter):
                s = get_option(opt)
                if s == "":
                    return
                print(setter, s)
                s = f"h.{setter}(\"{s}\")"
                eval(s, None, {"h": h})

            set_if_not_empty("xtitle", "GetXaxis().SetTitle")
            set_if_not_empty("ytitle", "GetYaxis().SetTitle")

    h.Draw(drawopt)
    if show_title:
        draw_label(h.GetTitle())
    saveas = path.join(out_path, f"{f_tag}.{extension}")
    print(saveas)
    can.SaveAs(saveas)
    if f_tag in object_drawn:
        warning_msg("Replacing", f_tag)
    object_drawn[f_tag]
    f.Close()


def main(tag="qc",
         main_path="/tmp/",
         draw_only=["mDeltaXEtaCONSTR"],
         filename="snapshot.root",
         config_file="drawconfig.conf"):
    config_parser = configparser.RawConfigParser()
    config_parser.read(config_file)
    p = path.join(main_path, tag)
    if not path.isdir(p):
        raise ExecError("Cannot find directory", p)
    files = [f for f in glob.glob(
        path.join(p, f"**/{filename}"), recursive=True)]
    for fn in files:
        process = True
        if draw_only is not None:
            process = False
            for i in draw_only:
                if i not in fn:
                    continue
                process = True
        if not process:
            continue
        draw(fn, configuration=config_parser)


if __name__ == "__main__":
    parser = get_default_parser("Fetch data from CCDB"
                                "Basic example: `./draw.py`")

    parser.add_argument('--only', "-O",
                        type=str,
                        default=None,
                        nargs="+",
                        help='Names of the objects to draw exclusively')

    args = parser.parse_args()
    main(draw_only=args.only)
    input("press enter to continue")
