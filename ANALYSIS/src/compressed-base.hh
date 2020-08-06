/// @author Roberto Preghenella
/// @since  2020-07-26

#include "Framework/Task.h"
#include "Framework/WorkflowSpec.h"
#include "Framework/ConfigParamSpec.h"
#include "Framework/ConfigParamRegistry.h"
#include "Framework/ControlService.h"
#include "Framework/CallbackService.h"
#include "Framework/ConcreteDataMatcher.h"
#include "Framework/RawDeviceService.h"
#include "Framework/DeviceSpec.h"
#include <fairmq/FairMQDevice.h>

#include "Headers/RAWDataHeader.h"
#include "DataFormatsTOF/CompressedDataFormat.h"

#include "TOFReconstruction/DecoderBase.h"

using namespace o2::framework;

namespace o2 
{
namespace tof
{
namespace compressed
{

class CompressedBaseTask : public DecoderBaseT<o2::header::RAWDataHeaderV6>, public Task
{
 public:
  CompressedBaseTask() = default;
  ~CompressedBaseTask() override = default;

  void init(InitContext& ic) override;
  void run(ProcessingContext& pc) final;
  
 private:

  bool mStatus = false;
};

void
CompressedBaseTask::init(InitContext& ic)
{
  LOG(INFO) << "CompressedBaseTask init";
  DecoderBaseT<o2::header::RAWDataHeaderV6>::setDecoderCONET(true);
  
  auto finishFunction = []() {
    LOG(INFO) << "CompressedBaseTask finish";
  };
  ic.services().get<CallbackService>().set(CallbackService::Id::Stop, finishFunction);
};

void
CompressedBaseTask::run(ProcessingContext& pc)
{
  
  /** check status **/
  if (mStatus) {
    pc.services().get<ControlService>().readyToQuit(QuitRequest::Me);
    return;
  }

  /** loop over inputs routes **/
  for (auto iit = pc.inputs().begin(), iend = pc.inputs().end(); iit != iend; ++iit) {
    if (!iit.isValid())
      continue;

    /** loop over input parts **/
    for (auto const& ref : iit) {

      const auto* headerIn = DataRefUtils::getHeader<o2::header::DataHeader*>(ref);
      auto payloadIn = ref.payload;
      auto payloadInSize = headerIn->payloadSize;

      DecoderBaseT<o2::header::RAWDataHeaderV6>::setDecoderBuffer(payloadIn);
      DecoderBaseT<o2::header::RAWDataHeaderV6>::setDecoderBufferSize(payloadInSize);
      DecoderBaseT<o2::header::RAWDataHeaderV6>::run();
    }
  }
};
