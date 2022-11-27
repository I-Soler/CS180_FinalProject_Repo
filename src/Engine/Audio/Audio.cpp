
#include "Audio.h"
#include <Debug/AEXDebug.h>
#include "AudioSourceComp.h"
namespace AEX
{

#define MAX_VOICE_COUNT 100	// ! CANNOT BE ZERO

#pragma region// Voice

	//Voice::Voice() : pChannel(NULL), isLooping(false), isPaused(true), volume(1.0f), groupID(-1)
	//{}
	//Voice::~Voice()
	//{}
	//void Voice::SetVolume(float vol)
	//{
	//	if (pChannel == NULL)
	//		return;
	//
	//	volume = vol;
	//
	//	if (volume < 0.0f)
	//		volume = 0.0f;
	//	else if (volume > 1.0f)
	//		volume = 1.0f;
	//
	//	pChannel->setVolume(volume);
	//}
	//float  Voice::GetVolume()
	//{
	//	return volume;
	//}
	//void Voice::SetPause(bool paused)
	//{
	//	if (pChannel == NULL)
	//		return;
	//	isPaused = paused;
	//	pChannel->setPaused(paused);
	//}
	//bool Voice::IsPaused()
	//{
	//	return isPaused;
	//}
	//void Voice::SetLoop(bool isLoop)
	//{
	//	if (pChannel == NULL)
	//		return;
	//
	//	isLooping = isLoop;
	//	if (isLooping)
	//		pChannel->setLoopCount(-1);
	//	else
	//		pChannel->setLoopCount(0);
	//}
	//bool Voice::IsValid()
	//{
	//	if (pChannel == NULL)
	//		return false;
	//
	//	bool pl;
	//	pChannel->isPlaying(&pl);
	//	return pl;
	//}
	//bool Voice::IsLooping()
	//{
	//	return isLooping;
	//}
	//void Voice::SetGroupId(unsigned group)
	//{
	//	groupID = group;
	//}
	//unsigned Voice::GetGroupId()
	//{
	//	return groupID;
	//}
#pragma endregion

#pragma region// Sound

	Sound::Sound() : pSound(NULL)
	{}
#pragma endregion

	// ----------------------------------------------------------------------------
#pragma region// Audio Manager

	bool	AudioSystem::Initialize()
	{
		FMOD_RESULT result;

		result = FMOD::System_Create(&pFMOD);
		if (result != FMOD_OK)
		{
			//AE_ASSERT_MESG(result == FMOD_OK, "AUDIO: Error creating FMOD system! (%d) %s\n", result, FMOD_ErrorString(result));
			return false;
		}

		// Initialize FMOD
		result = pFMOD->init(MAX_VOICE_COUNT, FMOD_INIT_NORMAL, 0);
		if (result != FMOD_OK)
		{
			pFMOD->release();
			pFMOD = nullptr;
			return false;
		}

		//AllocVoices();
		mGlobalVol = 1.0f;
		return true;
	}
	void	AudioSystem::Shutdown()
	{
		if (nullptr == pFMOD)
			return;

		pFMOD->release();
		pFMOD = nullptr;
		//FreeVoices();
	}
	//void	AudioSystem::Update()
	//{
	//	if (NULL == pFMOD)
	//		return;
	//
	//	// Update FMOD -> play audio
	//	pFMOD->update();
	//
	//	// loop through the voices
	//	for (Voice::PTR_LIST::iterator it = usedVoices.begin(); it != usedVoices.end(); ++it)
	//	{
	//		// this voice is no longer playing
	//		if (!(*it)->IsValid())
	//		{
	//			// erase and push to free list
	//			freeVoices.push_back(*it);
	//			it = usedVoices.erase(it);
	//			if (it == usedVoices.end())
	//				break;
	//		}
	//	}
	//
	//}
	void	AudioSystem::Update()
	{
		pFMOD->update();

		// AudioSourceComp has no Update for the moment
		//for (auto& aud : mAudioSources)
		//	aud->Update();
	}
	void AudioSystem::AddAudioComp(AudioSourceComp* audioComp)
	{
		mAudioSources.remove(audioComp);	// prevent duplicates
		mAudioSources.push_back(audioComp);
	}
	void AudioSystem::RemoveAudioComp(AudioSourceComp* audioComp)
	{
		mAudioSources.remove(audioComp);
	}
	FMOD::System* AudioSystem::GetFMOD()
	{
		return this->pFMOD;
	}
	void AudioSystem::SetGroupVolume(unsigned group_id, float volume)
	{
		for (auto it = mAudioSources.begin(); it != mAudioSources.end(); ++it)
			if ((*it)->GetGroupId() == group_id)
				(*it)->SetVolume(volume);
	}
	//void AudioSystem::StopGroup(unsigned group_id)
	//{
	//	// loop over the voices
	//	for (auto it = usedVoices.begin(); it != usedVoices.end();)
	//	{
	//		auto voice = *it;
	//
	//		// if group matches
	//		if (voice->groupID == group_id)
	//		{
	//			voice->pChannel->stop();
	//			freeVoices.push_back(voice);
	//			it = usedVoices.erase(it);
	//			if (it == usedVoices.end())
	//				return;
	//		}
	//		else ++it;
	//	}
	//}
	// ----------------------------------------------------------------------------

	//void AudioSystem::FreeThisVoice(AEX::AudioSourceComp* pVoice)
	//{
	//	if (NULL == pFMOD)
	//		return;
	//	if (pVoice == NULL)
	//		return;
	//
	//	pVoice->pChannel->stop();
	//	//usedVoices.remove(pVoice);
	//	//freeVoices.push_back(pVoice);
	//}
	//unsigned AudioSystem::GetFreeVoiceCount() const
	//{
	//	return (unsigned)freeVoices.size();
	//}
	//unsigned AudioSystem::GetUsedVoiceCount() const
	//{
	//	return (unsigned)usedVoices.size();
	//}
	unsigned AudioSystem::GetTotalAudioSourceCount() const
	{
		return static_cast<unsigned>(mAudioSources.size());
	}
	const AudioSystem::SoundMap& AudioSystem::GetAllLoadedSounds() const
	{
		return soundResources;
	}

	// Restore voice from freeVoices list to usedVoices list
	//Voice* AudioSystem::GetFreeVoice()
	//{
	//	if (NULL == pFMOD)
	//		return NULL;
	//	if (freeVoices.empty())
	//		return NULL;
	//
	//	Voice* pv = freeVoices.back();
	//	freeVoices.pop_back();
	//	usedVoices.push_back(pv);
	//
	//	return pv;
	//}

	// Allocate all empty voices into freeVoices list
	//void	AudioSystem::AllocVoices()
	//{
	//	if (NULL == pFMOD)
	//		return;
	//
	//	// free the voices if necessary
	//	FreeVoices();
	//
	//	// allocate new array of voices
	//	voiceCount = MAX_VOICE_COUNT;
	//	voices = new Voice[voiceCount];
	//
	//	// push all onto the free voices
	//	for (unsigned int i = 0; i < voiceCount; ++i)
	//		freeVoices.push_back(voices + i);
	//}

	// Delete all voices
	//void	AudioSystem::FreeVoices()
	//{
	//	if (NULL == pFMOD)
	//		return;
	//	if (voices)
	//	{
	//		delete[] voices;
	//		voiceCount = 0;
	//		freeVoices.clear();
	//		usedVoices.clear();
	//	}
	//}

	// ----------------------------------------------------------------------------
	Sound* AudioSystem::CreateSound(const char* filename)
	{
		if (nullptr == pFMOD)
			return nullptr;

		// check if the sound doesn't exits already
		SoundMap::iterator it = soundResources.find(filename);
		if (it != soundResources.end()) {
			return it->second;
		}

		// Allocate new memory for the sound
		Sound* pSound = aexFactory->Create<Sound>();
		pFMOD->createSound(filename, FMOD_LOOP_NORMAL | FMOD_2D, 0, &pSound->pSound);

		// save the name of the file
		pSound->filename = filename;

		// error check
		if (pSound->pSound == nullptr)
		{
			// make sure to delete the sound pointer
			delete pSound;
			return nullptr;
		}

		// All is good

		// add to sound map
		soundResources[filename] = pSound;
		++mSoundCount;	// Stats update
		return pSound;
	}

	void		AudioSystem::FreeSound(Sound* pSound)
	{
		if (nullptr == pFMOD)
			return;
		if (pSound == nullptr)
			return;

		if (pSound->pSound != nullptr)
		{
			pSound->pSound->release();
			pSound->pSound = nullptr;
		}

		// remove from sound resources
		soundResources.erase(pSound->filename);

		// Stats update
		--mSoundCount;
		delete pSound;
	}
	FMOD::Channel* AudioSystem::Play(Sound* pSound, bool paused)
	{
		// make sure we can actually play the sound
		if (pFMOD == nullptr)
			return nullptr;

		FMOD::Channel* pChannel = nullptr;
		auto res = pFMOD->playSound(pSound->pSound, 0, paused, &pChannel);
		if (res != FMOD_OK)
			DebugPrint("Couldn't play Sound %s", pSound->GetName());

		return pChannel;
	}
	//Voice* AudioSystem::Play(Sound* pSound, bool paused, unsigned group_id)
	//{
	//	// make sure we can actually play the sound
	//	if (pFMOD == NULL || pSound == NULL)
	//		return NULL;
	//
	//	// look for a free voice
	//	AEX::AudioSourceComp* pVoice = GetFreeVoice();
	//
	//	// this voice is valid
	//	if (pVoice)
	//	{
	//		// we found an available voice
	//		FMOD_RESULT res = pFMOD->playSound(pSound->pSound, 0, paused, &pVoice->pChannel);
	//		pVoice->SetPause(paused);
	//		pVoice->SetLoop(false);
	//		pVoice->groupID = group_id;
	//	}
	//
	//	// Return the voice (either NULL or correct)
	//	return pVoice;
	//}
	//Voice* AudioSystem::Loop(Sound* pSound, bool paused, unsigned group_id)
	//{
	//	Voice* pv = Play(pSound, paused, group_id);
	//	if (pv)
	//		pv->SetLoop(true);
	//	return pv;
	//}
	// Voices from usedVoices list to freeVoices list
	//void		AudioSystem::StopAll()
	//{
	//	if (NULL == pFMOD)
	//		return;
	//
	//	// loop through the voices
	//	while (!usedVoices.empty())
	//	{
	//		// erase and push to free list
	//		usedVoices.back()->pChannel->stop();
	//		freeVoices.push_back(usedVoices.back());
	//		usedVoices.pop_back();
	//	}
	//}
	void		AudioSystem::StopAll()
	{
		if (nullptr == pFMOD)
			return;
		
		// loop through the audio sources
		for (auto it = mAudioSources.begin(); it != mAudioSources.end(); ++it)
			(*it)->SetPause(true);
	}

#pragma endregion
}