#pragma once

#include <../fmod/inc/fmod.h>
#include <Core/AEXCore.h>
#include <Composition/AEXComponent.h>
#include "Audio.h"


namespace AEX
{
	struct Sound;
	template <typename T> class TResource;
	class AudioSourceComp : public IComp
	{
		AEX_RTTI_DECL(AudioSourceComp, IComp);

		// friends have access to privates
		friend class AudioSystem;
	
	public:
		AudioSourceComp();
		void OnCreate()override;
		//void Initialize()override;
		//void Update()override;
		//void Shutdown()override;

		void AddToSystem()override;
		void RemoveFromSystem()override;

		void SetSound(TResource<Sound>* pSoundRes);
		void Play();
		void Loop();
		void Play(Sound* pSound);
		void Loop(Sound* pSound);
		void Play(Sound* pSound, bool paused);
		void Loop(Sound* pSound,bool paused);

		bool Edit();

		// volume
		float	GetVolume();
		void	SetVolume(float);
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
	
		void PlaySingleShot(TResource<Sound>* pSoundRes);

		enum { BACKGROUND = 0, SFX, GROUP_COUNT };	// for mGroupID
		unsigned soundId = 0;

		virtual void StreamRead(const nlohmann::json& j);
		virtual void StreamWrite(nlohmann::json& j) const;

	private:
		TResource<Sound> *	mSound = nullptr;
		FMOD::Channel*		mpChannel;	// pointer to the internal FMOD channel.
		float				mVolume;		// volume of this voice in normalized range [0,1]
		bool				mIsLooping;	// specifies if the voice is looping or not
		bool				mIsPaused;	// specifies if the voice is paused or not
		bool				mAllowMultiple = false;
		unsigned			mGroupID;	// specifies which group this channel belongs to. -1 means no specific group. 
	};
}