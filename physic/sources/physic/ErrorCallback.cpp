#include "ErrorCallback.h"
#include "core/CLog.h"

using namespace physx;

namespace Physics
{
    void ErrorCallback::reportError(const PxErrorCode::Enum code, const char* message, const char*, const int)
    {
        if (code == PxErrorCode::eNO_ERROR)
            return;

        if (code == PxErrorCode::eINTERNAL_ERROR || code == PxErrorCode::eINVALID_PARAMETER || code ==
            PxErrorCode::eINVALID_OPERATION || code == PxErrorCode::eOUT_OF_MEMORY || code == PxErrorCode::eABORT)
        {
            ASSERT(false, message, Core::ELogChannel::CLOG_PHYSICS);
        }
        else if(code == PxErrorCode::eDEBUG_WARNING || code == PxErrorCode::ePERF_WARNING)
        {
            LOG(LOG_WARNING, message, Core::ELogChannel::CLOG_PHYSICS);
        }
        else if(code == PxErrorCode::eDEBUG_INFO)
        {
            LOG(LOG_INFO, message, Core::ELogChannel::CLOG_PHYSICS);
        }
    }
}
