#ifndef _NETWORK_MANAGER_H_
#define _NETWORK_MANAGER_H_

#include <stdafx.h>
#include <Singleton.h>

#define NETWORK_MANAGER ssurge::NetworkManager::getSingletonPtr()

#define NETWORK_MANAGER_PORT 37326
#define NETWORK_MANAGER_SERVER_MAX_CLIENT_COUNT 10
#define NETWORK_MANAGER_NON_STRICT_DEFAULT_MAX_TIMER_VALUE 1.0

namespace ssurge
{
	class NetworkManager : public Singleton<NetworkManager>
	{
	/***** ATTRIBUTES *****/
	protected:
		/// Determines whether the transaction of data between the clients and
		/// the server should be done strictly (frequently and assuredly) or not
		bool mIsStrict;

		/// Determines whether the application which owns this network manager
		/// is a server or a client
		bool mIsServer;

		/// Determines whether the application which owns this network manager
		/// is playing multiplayer or not
		bool mIsLocal;

		/// Countdown timer for non-strict network managers (useless when strict)
		double mTimer;

		/// Maximum value for the countdown timer (useless when strict)
		double mTimerMax;

		/// The number of clients connected to this server (useless when client)
		unsigned int mClientCount;

		/// The port
		unsigned short mPort;

		/// The socket
		sf::UdpSocket* mSocket;

		/// The relevant IP addresses
		sf::IpAddress* mIPs;

		/// Recevied packets since the last update
		std::vector<sf::Packet*> mPackets;

		/// Packet reception candidate
		sf::Packet* mPacketReceptionCandidate;

	/***** CONSTRUCTORS / DESTRUCTORS *****/
	public:
		/// Default constructor
		NetworkManager(bool local, bool server, bool strict);

		/// Default destructor
		virtual ~NetworkManager();


	/***** METHODS *****/
	public:
		/// Called by the constructor to initialize members
		virtual int initialize();
		
		/// Called by the owning application to signal an update check
		void update(double dt);
		
		/// Called by the update check wrapping method to perform an actual update
		/// Should be implemented so that data transaction occurs through the socket objects
		virtual void update();

		/// Returns the countdown timer value (useless when strict)
		double getTimerValue() { return mTimer; }

		/// Returns the maximum countdown timer value (useless when strict)
		double getTimerMaxValue() { return mTimerMax; }

		/// Sets the maximum countdown timer value (useless when strict)
		void setTimerMaxValue(double v) { mTimerMax = v; }

	};
}

#endif

