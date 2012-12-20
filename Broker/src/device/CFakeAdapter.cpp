////////////////////////////////////////////////////////////////////////////////
/// @file           CFakeAdapter.cpp
///
/// @author         Michael Catanzaro <michael.catanzaro@mst.edu>
///
/// @project        FREEDM DGI
///
/// @description    Interface for an adapter that replaces states with commands.
///
/// These source code files were created at Missouri University of Science and
/// Technology, and are intended for use in teaching or research. They may be
/// freely copied, modified, and redistributed as long as modified versions are
/// clearly marked as such and this notice is not removed. Neither the authors
/// nor Missouri S&T make any warranty, express or implied, nor assume any legal
/// responsibility for the accuracy, completeness, or usefulness of these files
/// or any information distributed with these files.
///
/// Suggested modifications or questions about these files can be directed to
/// Dr. Bruce McMillin, Department of Computer Science, Missouri University of
/// Science and Technology, Rolla, MO 65409 <ff@mst.edu>.
////////////////////////////////////////////////////////////////////////////////

#include "CLogger.hpp"
#include "device/CFakeAdapter.hpp"

namespace freedm {
namespace broker {
namespace device {

namespace {

/// This file's logger.
CLocalLogger Logger(__FILE__);

}

////////////////////////////////////////////////////////////////////////////////
/// Creates a new fake device adapter adapter.
///
/// @return a smart pointer to the new device adapter.
////////////////////////////////////////////////////////////////////////////////
CFakeAdapter::Pointer CFakeAdapter::Create()
{
    Logger.Trace << __PRETTY_FUNCTION__ << std::endl;
    return Pointer(new CFakeAdapter);
}

////////////////////////////////////////////////////////////////////////////////
/// Starts the fake adapter. Actually does nothing.
////////////////////////////////////////////////////////////////////////////////
void CFakeAdapter::Start()
{
    Logger.Trace << __PRETTY_FUNCTION__ << std::endl;
    return /*nothing*/;
}

////////////////////////////////////////////////////////////////////////////////
/// Sets the value of a device's setting. If the device is not currently
/// registered with the adapter, it is added. If the setting does not
/// currently exist, it is added with a default value of 0.0
///
/// @param device the unique identifier of the target device.
/// @param key the desired setting on the target device.
///
/// @return the value of the requested setting.
////////////////////////////////////////////////////////////////////////////////
SignalValue CFakeAdapter::Get(const std::string device,
                              const std::string key) const
{
    Logger.Trace << __PRETTY_FUNCTION__ << std::endl;
    
    // Get a map of keys/values from the map of devices/maps.
    // Then look up the value in that map.
    DeviceMap::iterator deviceIter = m_registry.find(device);
    
    if (deviceIter == m_registry.end()) {
        KeyMap km;
        km.insert(std::make_pair(key, 0.0));
        m_registry.insert(std::make_pair(device, km));
        return 0.0;
    }
    
    KeyMap::iterator keyIter = deviceIter->second.find(key);
    if (keyIter == deviceIter->second.end()) {
        m_registry[device].insert(std::make_pair(key, 0.0));
        return 0.0;
    }
    
    return keyIter->second;
}

////////////////////////////////////////////////////////////////////////////////
/// Sets the value of a device's setting.
///
/// @param device the unique identifier of the target device.
/// @param key the desired setting on the target device.
/// @param value the new value for the setting to take.
////////////////////////////////////////////////////////////////////////////////
void CFakeAdapter::Set(const std::string device, const std::string key,
                       const SignalValue value)
{
    Logger.Trace << __PRETTY_FUNCTION__ << std::endl;
    m_registry[device][key] = value;
}

} // namespace device
} // namespace broker
} // namespace freedm
