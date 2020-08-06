/// @author Roberto Preghenella
/// @since  2020-07-26

#include "compressed-template.hh"

#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"

using namespace o2::framework;

namespace o2 
{
namespace tof
{
namespace compressed
{

class CompressedTemplateTask : public CompressedBaseTask
{
public:
  CompressedTemplateTask() = default;
  ~CompressedTemplateTask() = default;

  void init(InitContext& ic) override;
  
private:

  void headerHandler(const CrateHeader_t* crateHeader, const CrateOrbit_t* crateOrbit) final;
  
  void frameHandler(const CrateHeader_t* crateHeader, const CrateOrbit_t* crateOrbit,
                    const FrameHeader_t* frameHeader, const PackedHit_t* packedHits) final;

  void trailerHandler(const CrateHeader_t* crateHeader, const CrateOrbit_t* crateOrbit,
                      const CrateTrailer_t* crateTrailer, const Diagnostic_t* diagnostics,
                      const Error_t* errors) final;  

  int mDiagnostics[12][32];

};

void
CompressedTemplateTask::init(InitContext& ic)
{
  LOG(INFO) << "CompresseTemplateTask init";
  DecoderBaseT<o2::header::RAWDataHeaderV6>::setDecoderCONET(true);
  
  for (int i = 0; i < 12; ++i)
    for (int j = 0; j < 32; ++j)
      mDiagnostics[i][j] = 0;

  auto finishFunction = [this]() {
    LOG(INFO) << "CompresseTemplateTask finish";
    TFile fout("compressed-template.root", "RECREATE");
    for (int i = 0; i < 12; ++i) {
      TH1F h(Form("hDiagnostics_%02d", i + 1), ";bit", 32, 0., 32.);
      for (int j = 0; j < 32; ++j)
	h.SetBinContent(j + 1, mDiagnostics[i][j]);
      h.Write();
    }
    fout.Close();
  };
  ic.services().get<CallbackService>().set(CallbackService::Id::Stop, finishFunction);
};



void CompressedTemplateTask::headerHandler(const CrateHeader_t* crateHeader, const CrateOrbit_t* crateOrbit)
{
  //  LOG(INFO) << " --- headerHandler ";
  mDiagnostics[0][0]++;
  auto slotPartMask = crateHeader->slotPartMask;
  for (int ibit = 0; ibit < 11; ++ibit)
    if (slotPartMask & (1 << ibit))
      mDiagnostics[ibit + 1][0]++;
}

void CompressedTemplateTask::frameHandler(const CrateHeader_t* crateHeader, const CrateOrbit_t* crateOrbit,
					  const FrameHeader_t* frameHeader, const PackedHit_t* packedHits)
{
  //  LOG(INFO) << " --- frameHandler ";
}

void CompressedTemplateTask::trailerHandler(const CrateHeader_t* crateHeader, const CrateOrbit_t* crateOrbit,
					    const CrateTrailer_t* crateTrailer, const Diagnostic_t* diagnostics,
					    const Error_t* errors)
{
  //  LOG(INFO) << " --- trailerHandler ";
  for (int i = 0; i < crateTrailer->numberOfDiagnostics; ++i) {
    auto diagnostic = diagnostics + i;
    auto slotID = diagnostic->slotID;
    auto faultBits = diagnostic->faultBits;
    for (int ibit = 0; ibit < 28; ++ibit)
      if (faultBits & (1 << ibit))
	mDiagnostics[slotID - 1][ibit + 4]++;
  }

}

} /** namespace compressed **/
} /** namespace tof **/
} /** namespace o2 **/

// add workflow options, note that customization needs to be declared before
// including Framework/runDataProcessing
void customize(std::vector<ConfigParamSpec>& workflowOptions)
{}

#include "Framework/runDataProcessing.h" // the main driver

/// This function hooks up the the workflow specifications into the DPL driver.
WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec {
    DataProcessorSpec {"compressed-template",
	select("x:TOF/CRAWDATA"),
	Outputs{},
	AlgorithmSpec(adaptFromTask<o2::tof::compressed::CompressedTemplateTask>()),
	Options{
	  {"compressed-template-conet-mode", VariantType::Bool, false, {"CONET mode"}}}
    }
  };
}

