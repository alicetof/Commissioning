#!/usr/bin/env python3

"""
Script to plot the NSigma, extract resolution and parameters
"""


from debugtrack import DebugTrack
from ROOT import o2, TFile, TF1, gROOT, gInterpreter, TPaveText
from plotting import draw_nice_canvas, update_all_canvases, draw_nice_legend
from common import get_default_parser, set_verbose_mode


def main(fname, paramn="/tmp/tofreso.root", hn="Pi", rebinx=-2):
    f = TFile(fname, "READ")
    if not f.IsOpen():
        print("Cannot open", fname)
    f.ls()
    f.Get("tof-pid-qa/nsigma").ls()
    # d.ls()
    hdelta = f.Get(f"tof-pid-qa/expected_diff/{hn}")
    hdelta.SetDirectory(0)
    hdelta.SetName("hdelta")
    hnsigma = f.Get(f"tof-pid-qa/nsigma/{hn}")
    hnsigma.SetDirectory(0)
    hnsigma.SetName("hnsigma")
    f.Close()

    if "TH3" in hdelta.ClassName():
        if 1:
            draw_nice_canvas("3d", logz=False)
            hdelta.DrawClone("COLZ")
            update_all_canvases()

        hdelta = hdelta.Project3D("yx")

        hnsigma = hnsigma.Project3D("yx")
    if rebinx > 0:
        hdelta.RebinX(rebinx)
        hnsigma.RebinX(rebinx)

    draw_nice_canvas("nsigma", logz=True)
    hnsigma.Draw("COLZ")

    fgaus = TF1("gaus", "gaus", -200, 200)

    draw_nice_canvas("nsigmaslice")
    bins = [hnsigma.GetXaxis().FindBin(0.9), hnsigma.GetXaxis().FindBin(1.1)]
    slicensigma1gev = hnsigma.ProjectionY(f"{bins[0]}-{bins[1]}", *bins)
    slicensigma1gev.SetDirectory(0)
    slicensigma1gev.Fit(fgaus)
    slicensigma1gev.Draw()
    pavensigma = TPaveText(.68, .63, .91, .72, "brNDC")
    pavensigma.AddText("#mu = {:.3f}".format(fgaus.GetParameter(1)))
    pavensigma.AddText("#sigma = {:.3f}".format(fgaus.GetParameter(2)))
    pavensigma.Draw()

    hparmu = hdelta.ProjectionX("mu")
    hparmu.Clear()
    hparmu.SetLineColor(2)
    hparmu.SetDirectory(0)

    hparsigma = hdelta.ProjectionX("sigma")
    hparsigma.Clear()
    hparsigma.SetLineColor(3)
    hparsigma.SetDirectory(0)

    debugtrack = DebugTrack()
    # debugtrack = o2.pid.tof.DebugTrack()
    tofpar = o2.pid.tof.TOFResoParams()
    # exptimes = o2.pid.tof.ExpTimes(o2.pid.tof.DebugTrack, 2)
    exptimes = o2.pid.tof.ExpTimes(DebugTrack, 2)
    print(exptimes.GetExpectedSigma(tofpar, debugtrack))

    def param(x, parameters):
        debugtrack.mp = x[0]
        tofpar.SetParameter(0, parameters[0])
        tofpar.SetParameter(1, parameters[1])
        tofpar.SetParameter(2, parameters[2])
        tofpar.SetParameter(3, parameters[3])
        tofpar.SetParameter(4, parameters[4])
        # tofpar.Print()
        collisionTimeRes = 20.0
        tofSignal = 1.
        return exptimes.GetExpectedSigma(tofpar, debugtrack, tofSignal, collisionTimeRes)

    for i in range(1, hdelta.GetNbinsX()+1):
        s = hdelta.ProjectionY(f"{i}", i, i)
        s.SetDirectory(0)
        s.Fit(fgaus, "QNR")
        hparmu.SetBinContent(i, fgaus.GetParameter(1))
        hparsigma.SetBinContent(i, fgaus.GetParameter(2))

    draw_nice_canvas("slice")
    bins = [hdelta.GetXaxis().FindBin(0.9), hdelta.GetXaxis().FindBin(1.1)]
    slice1gev = hdelta.ProjectionY(f"{bins[0]}-{bins[1]}", *bins)
    slice1gev.SetDirectory(0)
    slice1gev.Fit(fgaus)
    slice1gev.Draw()
    pave = TPaveText(.68, .63, .91, .72, "brNDC")
    pave.AddText("#mu = {:.3f}".format(fgaus.GetParameter(1)))
    pave.AddText("#sigma = {:.3f}".format(fgaus.GetParameter(2)))
    pave.Draw()

    draw_nice_canvas("fit")
    hdelta.Draw("COLZ")
    hparmu.Draw("same")
    hparsigma.Draw("same")
    if paramn is not None:
        tofpar.LoadParamFromFile(paramn, "TOFResoParams")

    fresoorig = TF1("paramorig", param, 0.1, 1.5, tofpar.size())
    for i in range(tofpar.size()):
        fresoorig.SetParameter(i, tofpar.GetParameter(i))
    fresoorig.SetLineColor(1)
    fresoorig.Draw("same")
    sorig = ""
    for i in range(0, fresoorig.GetNpar()):
        sorig += " --p{} {:.2f}".format(i, fresoorig.GetParameter(i))

    freso = TF1("param", param, 0.1, 1.5, tofpar.size())
    freso.SetParLimits(0, 0, 0.1)
    freso.SetParLimits(1, 0, 0.1)
    freso.SetParLimits(2, 0, 0.1)
    freso.SetParLimits(3, 0, 200)
    freso.SetParLimits(4, 0, 200)
    hparsigma.Fit(freso, "QNR", "", 0.2, 2)
    freso.Draw("same")
    leg = draw_nice_legend()
    leg.AddEntry(fresoorig, "Starting")
    leg.AddEntry(freso, "Refitted")
    s = ""
    for i in range(0, freso.GetNpar()):
        s += " --p{} {:.2f}".format(i, freso.GetParameter(i))
    print("Starting calib", sorig)
    print("Post calib", s)

    update_all_canvases()

    input("press enter to continue")


if __name__ == "__main__":
    parser = get_default_parser(__doc__ +
                                "Basic example: `./fitreso.py paths/AnalysisResults.root`")
    parser.add_argument('files',
                        type=str,
                        nargs="+",
                        help='Input files')

    args = parser.parse_args()
    set_verbose_mode(args)

    for i in args.files:
        main(i)
