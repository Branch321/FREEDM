////////////////////////////////////////////////////////////////////////////////
/// @file           CAdapterFactory.hpp
///
/// @author         Thomas Roth <tprfh7@mst.edu>
/// @author         Michael Catanzaro <michael.catanzaro@mst.edu>
///
/// @project        FREEDM DGI
///
/// @description    Handles the creation of device adapters.
///
/// @functions
///     CAdapterFactory::RegisterDevicePrototype
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

#ifndef C_ADAPTER_FACTORY_HPP
#define C_ADAPTER_FACTORY_HPP

#include "CLogger.hpp"
#include "IDevice.hpp"
#include "IAdapter.hpp"
#include "CTcpServer.hpp"
#include "CDeviceManager.hpp"

#include <map>
#include <set>
#include <string>
#include <stdexcept>

#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/property_tree/ptree_fwd.hpp>

namespace freedm {
namespace broker {
namespace device {

namespace {
/// This file's logger.
CLocalLogger AdapterFactoryLogger(__FILE__);
}

/// Converts a preprocessor token into a templated function call.
#define REGISTER_DEVICE_PROTOTYPE(SUFFIX) \
RegisterDevicePrototype<CDevice##SUFFIX>(#SUFFIX)

/// Handles the creation of adapters and their associated devices.
////////////////////////////////////////////////////////////////////////////////
/// Singleton factory that creates, stores, and runs new device adapters.
///
/// @limitations This class is not thread safe.
////////////////////////////////////////////////////////////////////////////////
class CAdapterFactory
    : private boost::noncopyable
{
public:
    /// Gets the static instance of the factory.
    static CAdapterFactory & Instance();
    
    /// Destructs the factory.
    ~CAdapterFactory();
    
    /// Creates a new adapter and its associated devices.
    void CreateAdapter(const boost::property_tree::ptree & p);

    /// Removes an adapter and its associated devices.
    void RemoveAdapter(const std::string identifier);
    
    /// Adds a new TCP port number for adapters.
    void AddPortNumber(const unsigned short port);
private:
    /// Constructs the factory.
    CAdapterFactory();

    /// Registers compiled device classes with the factory.
    void RegisterDevices();
    
    /// Creates an instance of a device to use as a prototype.
    template <class DeviceType>
    void RegisterDevicePrototype(const std::string identifier);
    
    /// Runs the factory I/O service.
    void RunService();
    
    /// Clones a device prototype and registers it with the system.
    void CreateDevice(const std::string name, const std::string type, 
            IAdapter::Pointer adapter);
    
    /// Initializes the devices stored on an adapter.
    void InitializeAdapter(IAdapter::Pointer adapter,
            const boost::property_tree::ptree & p);

    /// Gets the next available TCP port number.
    unsigned short GetPortNumber();

    /// Session layer protocol for plug-and-play devices.
    void SessionProtocol(IServer::Pointer connection);
    
    /// Set of device prototypes managed by the factory.
    std::map<std::string, IDevice::Pointer> m_prototype;
    
    /// Set of adapters managed by the factory.
    std::map<std::string, IAdapter::Pointer> m_adapter;
    
    /// I/O service shared by the adapters.
    boost::asio::io_service m_ios;
    
    /// Thread to run the i/o service
    boost::thread m_thread;
    
    /// TCP server to accept plug-and-play devices.
    CTcpServer::Pointer m_server;
    
    /// Set of TCP port numbers.
    std::set<unsigned short> m_ports;
};

////////////////////////////////////////////////////////////////////////////////
/// Creates a new device of DeviceType and stores it as a prototype using the
/// provided string identifier.
///
/// @ErrorHandling Throws a std::runtime_error if the string identifier has
/// been used to register another device type.
/// @pre The identifier must not already be registered with the factory.
/// @post A device is created using a null adapter and stored in m_prototype.
/// @param identifier The string identifier to associate with the prototype.
///
/// @limitations None.
////////////////////////////////////////////////////////////////////////////////
template <class DeviceType>
void CAdapterFactory::RegisterDevicePrototype(const std::string identifier)
{
    AdapterFactoryLogger.Trace << __PRETTY_FUNCTION__ << std::endl;

    IAdapter::Pointer null;
    IDevice::Pointer dev;
    
    if( m_prototype.count(identifier) > 0 )
    {
        throw std::runtime_error("The string identifier " + identifier
                + " has already been registered with the adapter factory.");
    }

    dev = IDevice::Pointer(new DeviceType("prototype-" + identifier, null));
    m_prototype.insert(std::make_pair(identifier, dev));

    AdapterFactoryLogger.Info << "Created a new device prototype with the "
            << "string identifier: " << identifier << "." << std::endl;
}

} // namespace device
} // namespace freedm
} // namespace broker

#endif // C_ADAPTER_FACTORY_HPP
