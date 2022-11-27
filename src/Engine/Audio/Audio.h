// ---------------------------------------------------------------------------
// Project Name		:	Alpha Engine
// File Name		:	AEAudio.h
// Author			:	Thomas Komair
// Creation Date	:	2013/06/13
// Purpose			:	main header file for the AudioSystem
// History			:
// - 2012/03/29		:	- initial implementation
// ---------------------------------------------------------------------------

#pragma once

// STL includes
#include <../fmod/inc/fmod.hpp>
#include <list>
#include <map>
#include <string>
#include <Core/AEXCore.h>

// Forward declare the FMOD classes
// so that I don't have to include fmod.hpp 
// in Audio.h, and therefore create an unnecessary 
// dependency. 
namespace FMOD
{
	class Sound;
	class Channel;
	class System;
}
namespace AEX
{

	struct Sound : public IBase
	{
	private:
		AEX_RTTI_DECL(Sound, IBase);

	public:
		FMOD::Sound* pSound;
		std::string		 filename;

		Sound();
	};
	/*
	class Voice : public AEX::IBase
	{
		// friends have access to privates
		friend class AudioSystem;

	public:

		// constructors and destructors
		Voice();
		~Voice();

		// -----
		// Setters and Getters

		// volume
		float	GetVolume();
		void	SetVolume(float volume);

		// paused
		void	SetPause(bool paused);
		bool	IsPaused();

		// looping
		void	SetLoop(bool isLoop);
		bool	IsLooping();

		// group Id
		void	SetGroupId(unsigned group);
		unsigned GetGroupId();

		// Returns false when the voice finished playing.
		// Note: this is meant for the gameplay code to know when a sound finished playing.
		bool IsValid();

		// STL
		typedef std::list<Voice *> PTR_LIST;

	private:

		FMOD::Channel * pChannel;	// pointer to the internal FMOD channel.
		float			volume;		// volume of this voice in normalized range [0,1]
		bool			isLooping;	// specifies if the voice is looping or not
		bool			isPaused;	// specifies if the voice is paused or not
		unsigned		groupID;	// specifies which group this channel belongs to. -1 means no specific group.
	};
	*/
	class AudioSourceComp;
	class AudioSystem : public IBase
	{

		AEX_RTTI_DECL(AudioSystem, IBase);
		AEX_SINGLETON(AudioSystem) {}

		typedef std::map<std::string, Sound*> SoundMap;
	public:

		bool Initialize();
		void Update();
		void AddAudioComp(AudioSourceComp*);
		void RemoveAudioComp(AudioSourceComp*);

		void Shutdown();

		// Sound Management
		Sound* CreateSound(const char* filename);
		void	FreeSound(Sound* pSound);
		//Voice * Loop(Sound * pSound, bool paused = false, unsigned group_id = -1);
		FMOD::Channel * Play(Sound * pSound, bool paused = false);

		FMOD::System* GetFMOD();

		// set group volumes
		void SetGroupVolume(unsigned group_id, float volume);
		//void StopGroup(unsigned group_id);

		// Stop All
		void StopAll();
		//void FreeThisVoice(AEX::AudioSourceComp*pVoice);

		// Debug Stats
		//unsigned GetFreeVoiceCount() const;
		//unsigned GetUsedVoiceCount() const;
		unsigned GetTotalAudioSourceCount() const;
		const SoundMap& GetAllLoadedSounds() const;


	private:
		// Helper methods
		//void						AllocVoices();
		//void						FreeVoices();
		// Voice manager... adds to free and removes from used
		//AEX::AudioSourceComp*		GetFreeVoice();
		// FMOD System
		FMOD::System* pFMOD;
		// keep track of the sounds created 
		unsigned int				mSoundCount;
		// Voice Management List (equivalent to dead list and free list)
		std::list<AudioSourceComp*> mAudioSources;
		//AEX::AudioSourceComp*			voices;
		//AEX::AudioSourceComp::PTR_LIST	freeVoices;
		//AEX::AudioSourceComp::PTR_LIST	usedVoices;
		// Sound Management
		SoundMap					soundResources;
		float						mGlobalVol;
	};



#define aexAudio AEX::Singletons::Get<AudioSystem>()
}
