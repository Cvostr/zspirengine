#include <iostream>
#include <fstream>

#include "../../headers/audio/OALManager.hpp"
#include "../../headers/engine/Logger.hpp"


Engine::OALManager::OALManager() :
	mOpenALDevice(nullptr),
	mOpenALContext(nullptr) {}

Engine::OALManager::~OALManager() {
	destroyAL();
}

void Engine::OALManager::OnCreate() {
	initAL();
}

bool Engine::OALManager::initAL() {
	mOpenALDevice = alcOpenDevice(nullptr);
	if (!mOpenALDevice) {
		Logger::Log(LogType::LOG_TYPE_ERROR) << "AL: Can't initialize OpenAL device\n";
		return false;
	}

	mOpenALContext = alcCreateContext(mOpenALDevice, nullptr);

	if (!mOpenALContext) {
		Logger::Log(LogType::LOG_TYPE_ERROR) << "AL: Can't initialize OpenAL device context\n";
		return false;
	}

	alcMakeContextCurrent(mOpenALContext);

	if (alGetError() != AL_NO_ERROR)
	{
		Logger::Log(LogType::LOG_TYPE_ERROR) << "Can't initialize\n";
	}

	Logger::Log(LogType::LOG_TYPE_INFO) << "AL: OpenAL successfully initialized!\n";
	//Set default parameters
	setListenerPos(Vec3(0.0f, 0.0f, 0.0f));
	setListenerOri(Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 1.0f, 0.0f));
	setListenerVelocity(Vec3(0.0f, 0.0f, 0.0f));
	Engine::OALManager::setListenerVolume(1.0f);

	return true;
}

void Engine::OALManager::setListenerPos(Vec3 pos) {
	alListener3f(AL_POSITION, pos.X, pos.Y, pos.Z);
}
void Engine::OALManager::setListenerOri(Vec3 front, Vec3 up) {
	ALfloat listenerOri[] = { front.X, front.Y, front.Z, up.X, up.Y, up.Z };
	alListenerfv(AL_ORIENTATION, listenerOri);
}

void Engine::OALManager::setListenerVolume(float value) {
	alListenerf(AL_GAIN, value);
}

void Engine::OALManager::setListenerVelocity(Vec3 velocity) {
	alListener3f(AL_VELOCITY, velocity.X, velocity.Y, velocity.Z);
}

void Engine::OALManager::destroyAL() {
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(mOpenALContext);
	alcCloseDevice(mOpenALDevice);
}

