
#include<ResourceMgr/ResourceMgr.h>
#include "AudioSourceComp.h"
#include "imgui/imgui.h"
#include "Editor/AEXEditor.h"

namespace AEX
{
	AudioSourceComp::AudioSourceComp()
	{
		mSound = nullptr;
		mpChannel = nullptr;
		mVolume = 0.2f;
		mIsLooping = false;
		mIsPaused = false;
		mAllowMultiple = false;
		mGroupID = 0;
	}
	void AudioSourceComp::OnCreate()
	{
	}
	void AudioSourceComp::AddToSystem()
	{
		aexAudio->AddAudioComp(this);
	}
	void AudioSourceComp::RemoveFromSystem()
	{
		aexAudio->RemoveAudioComp(this);
	}
	void AudioSourceComp::SetSound(TResource<Sound>* pSoundRes)
	{
		mSound = pSoundRes;
	}
	void AudioSourceComp::Play()
	{
		Play(mSound->get());
	}
	void AudioSourceComp::Loop()
	{
		Loop(mSound->get());
	}
	void AudioSourceComp::Play(Sound* pSound)
	{
		Play(pSound, mIsPaused);
	}
	void AudioSourceComp::Loop(Sound* pSound)
	{
		Loop(pSound, mIsPaused);
	}
	void AudioSourceComp::Play(Sound* pSound, bool paused)
	{
		if (IsValid() && mAllowMultiple == false)return;

		mpChannel = aexAudio->Play(pSound, paused);
		SetVolume(mVolume);
		SetLoop(false);
		SetPause(paused);
	
		// we found an available sound
		//SetLoop(false);
	}
	void AudioSourceComp::Loop(Sound* pSound, bool paused)
	{
		if (IsValid() && mAllowMultiple == false)return;

		mpChannel = aexAudio->Play(pSound, paused);

		// we found an available sound
		SetPause(paused);
		SetLoop(true);
	}
	bool AudioSourceComp::Edit()
	{
		// display sound name
		std::string str;
		mSound == nullptr ? str = "No sound" : str = "Sound: " + std::string(mSound->GetName());
		ImGui::Text(str.c_str());

		// set sound
		TResource<Sound>* tSound = nullptr;
		if (ImGui::CollapsingHeader("Set sound"))
		{
			if (aexEditor->getResource<Sound>(&tSound))
			{
				SetSound(tSound);
				return true;
			}
		}

		if (ImGui::Button("Play"))
			Play();

		// set other variables
		if (ImGui::Checkbox("Pause", &mIsPaused))
			SetPause(mIsPaused);
		if (ImGui::Checkbox("Loop", &mIsLooping))
			SetLoop(mIsLooping);
		if (ImGui::SliderFloat("Volumen", &mVolume, 0.0f, 1.0f))
			SetVolume(mVolume);
		ImGui::Checkbox("Allow Multiple", &mAllowMultiple);
		
		return true;
	}
	float AudioSourceComp::GetVolume()
	{
		return mVolume;
	}
	
	void AudioSourceComp::SetVolume(float vol)
	{
		mVolume = Clamp(vol, 0.0f, 1.0f);

		if(IsValid())
			mpChannel->setVolume(mVolume);
	}
	
	void AudioSourceComp::SetPause(bool paused)
	{
		mIsPaused = paused;

		if (IsValid())
			mpChannel->setPaused(paused);
	}
	
	bool AudioSourceComp::IsPaused()
	{
		return mIsPaused;
	}
	
	void AudioSourceComp::SetLoop(bool isLoop)
	{
		mIsLooping = isLoop;

		if (IsValid())
		{
			if (mIsLooping)
			{
				mpChannel->setLoopCount(-1);
			}
			else
			{
				mpChannel->setLoopCount(0);
			}
		}
	}
	
	bool AudioSourceComp::IsLooping()
	{
		return mIsLooping;
	}
	
	void AudioSourceComp::SetGroupId(unsigned group)
	{
		mGroupID = group;
	}
	
	unsigned AudioSourceComp::GetGroupId()
	{
		return mGroupID;
	}
	
	bool AudioSourceComp::IsValid()
	{
		if (mpChannel == nullptr)
			return false;
	
		bool pl;
		mpChannel->isPlaying(&pl);
		return pl;
	}

	void AudioSourceComp::PlaySingleShot(TResource<Sound>* pSoundRes)
	{
		if (pSoundRes == nullptr)
			return;

		Sound* pSound = pSoundRes->get();
		pSound->pSound->setLoopCount(0);

		aexAudio->Play(pSound, false);
	}

	void AudioSourceComp::StreamRead(const nlohmann::json& j)
	{
		// read the sound name
		std::string tmp;
		tmp = j["soundName"];
		
		if (tmp == "null")
			mSound = nullptr;
		else
			mSound = aexResources->GetResource<Sound>(tmp.c_str());
	}
	void AudioSourceComp::StreamWrite(nlohmann::json& j) const
	{
		if (mSound && mSound->get())
		{
			j["soundName"] = mSound->GetName();
		}
		else 
			j["soundName"] = "null";
	}
}