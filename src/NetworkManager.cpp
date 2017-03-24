#include <stdafx.h>
#include <NetworkManager.h>

// Template specialization to initialize the network manager's singleton pointer
template<>
ssurge::NetworkManager * ssurge::Singleton<ssurge::NetworkManager>::msSingleton = nullptr;

ssurge::NetworkManager::NetworkManager(bool local, bool server, bool strict) :
	mIsStrict(strict),
	mIsServer(server),
	mIsLocal(local),
	mTimer(0.0),
	mTimerMax(NETWORK_MANAGER_NON_STRICT_DEFAULT_MAX_TIMER_VALUE),
	mClientCount(NETWORK_MANAGER_SERVER_MAX_CLIENT_COUNT),
	mPort(NETWORK_MANAGER_PORT),
	mSocket(NULL),
	mIPs(NULL),
	mPacketReceptionCandidate(NULL)
{
	initialize();
}

ssurge::NetworkManager::~NetworkManager()
{
	if (mIsLocal)
		return;

	if (mIsServer)
		delete[] mIPs;
	else
		delete mIPs;

	delete mPacketReceptionCandidate;

	for (unsigned int i = 0; i != mPackets.size(); ++i)
		delete mPackets[i];
	mPackets.clear();
}

int ssurge::NetworkManager::initialize()
{
	if (mIsLocal)
		return 1;

	mSocket = new sf::UdpSocket();
	if (mSocket->bind(mPort) != sf::Socket::Done)
		return 2;

	if (mIsServer)
		mIPs = new sf::IpAddress[NETWORK_MANAGER_SERVER_MAX_CLIENT_COUNT];
	else
		mIPs = new sf::IpAddress("192.168.1.1"); // Temporary router access address or whatever

	mPacketReceptionCandidate = new sf::Packet();

	return 0;
}

void ssurge::NetworkManager::update(double dt)
{
	if (mIsLocal)
		return;
	
	if (!mIsStrict)
	{
		mTimer -= dt;
		if (mTimer <= 0.0)
		{
			mTimer = mTimerMax;
			update();
		}
	}
	else
	{
		update();
	}
}

void ssurge::NetworkManager::update()
{
	if (mIsServer)
	{
		for (unsigned int i = 0; i != mClientCount; ++i)
		{
			for (unsigned int j = 0; j != mPackets.size(); ++j)
			{
				mSocket->send(*mPackets[i], mIPs[i], mPort);
			}
		}
	}
	else
	{

	}
}
