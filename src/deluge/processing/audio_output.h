/*
 * Copyright © 2019-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "definitions_cxx.hpp"
#include "model/global_effectable/global_effectable_for_clip.h"
#include "model/output.h"
#include "modulation/envelope.h"

class ModelStackWithTimelineCounter;

class AudioOutput final : public Output, public GlobalEffectableForClip {
public:
	AudioOutput();
	virtual ~AudioOutput();
	void cloneFrom(ModControllableAudio* other);

	void renderOutput(ModelStack* modelStack, StereoSample* startPos, StereoSample* endPos, int32_t numSamples,
	                  int32_t* reverbBuffer, int32_t reverbAmountAdjust, int32_t sideChainHitPending,
	                  bool shouldLimitDelayFeedback, bool isClipActive);

	bool renderGlobalEffectableForClip(ModelStackWithTimelineCounter* modelStack, StereoSample* globalEffectableBuffer,
	                                   int32_t* bufferToTransferTo, int32_t numSamples, int32_t* reverbBuffer,
	                                   int32_t reverbAmountAdjust, int32_t sideChainHitPending,
	                                   bool shouldLimitDelayFeedback, bool isClipActive, int32_t pitchAdjust,
	                                   int32_t amplitudeAtStart, int32_t amplitudeAtEnd);

	void resetEnvelope();

	ModControllable* toModControllable() { return this; }
	uint8_t* getModKnobMode() { return &modKnobMode; }

	void cutAllSound();
	void getThingWithMostReverb(Sound** soundWithMostReverb, ParamManagerForTimeline** paramManagerWithMostReverb,
	                            Kit** kitWithMostReverb, int32_t* highestReverbAmountFound);

	Error readFromFile(Deserializer& reader, Song* song, Clip* clip, int32_t readAutomationUpToPos);
	bool writeDataToFile(Serializer& writer, Clip* clipForSavingOutputOnly, Song* song);
	void deleteBackedUpParamManagers(Song* song);
	bool setActiveClip(ModelStackWithTimelineCounter* modelStack,
	                   PgmChangeSend maySendMIDIPGMs = PgmChangeSend::ONCE) override;
	bool isSkippingRendering();
	Output* toOutput() { return this; }
	void getThingWithMostReverb(Sound** soundWithMostReverb, ParamManager** paramManagerWithMostReverb,
	                            GlobalEffectableForClip** globalEffectableWithMostReverb,
	                            int32_t* highestReverbAmountFound);

	// A TimelineCounter is required
	void offerReceivedCCToLearnedParams(MIDIDevice* fromDevice, uint8_t channel, uint8_t ccNumber, uint8_t value,
	                                    ModelStackWithTimelineCounter* modelStack) {
		ModControllableAudio::offerReceivedCCToLearnedParamsForClip(fromDevice, channel, ccNumber, value, modelStack);
	}
	bool offerReceivedPitchBendToLearnedParams(MIDIDevice* fromDevice, uint8_t channel, uint8_t data1, uint8_t data2,
	                                           ModelStackWithTimelineCounter* modelStack) {
		return ModControllableAudio::offerReceivedPitchBendToLearnedParams(fromDevice, channel, data1, data2,
		                                                                   modelStack);
	}

	char const* getXMLTag() { return "audioTrack"; }

	Envelope envelope;

	int32_t amplitudeLastTime;

	int32_t overrideAmplitudeEnvelopeReleaseRate;

	/// Audio channel used for recording and monitoring.
	AudioInputChannel inputChannel{AudioInputChannel::UNSET};

	/// Only used during loading - index changes as outputs are added/removed and this won't get updated. Pointer stays
	/// accurate through those changes.
	///
	/// int16 so it packs nicely with `echoing` below
	int16_t outputRecordingFromIndex{-1}; // int16 so it fits with the bool and because that should be enough outputs
	/// When true, this output is monitoring its input.
	///
	/// Does not get copied when this Output is cloned, as that would result in undesirable doubling of the monitored
	/// audio.
	bool echoing;

	Output* getOutputRecordingFrom() { return outputRecordingFrom; }
	void clearRecordingFrom() override { setOutputRecordingFrom(nullptr, false); }
	void setOutputRecordingFrom(Output* toRecordfrom, bool monitoring) {
		if (outputRecordingFrom) {
			outputRecordingFrom->setRenderingToAudioOutput(false, nullptr);
		}
		outputRecordingFrom = toRecordfrom;
		if (outputRecordingFrom) {
			outputRecordingFrom->setRenderingToAudioOutput(monitoring, this);
		}
		echoing = monitoring;
	}

	ModelStackWithAutoParam* getModelStackWithParam(ModelStackWithTimelineCounter* modelStack, Clip* clip,
	                                                int32_t paramID, deluge::modulation::params::Kind paramKind,
	                                                bool affectEntire, bool useMenuStack);

protected:
	Clip* createNewClipForArrangementRecording(ModelStack* modelStack);
	bool wantsToBeginArrangementRecording();
	bool willRenderAsOneChannelOnlyWhichWillNeedCopying();

	/// Which output to record from. Only valid when inputChannel is AudioInputChannel::SPECIFIC_OUTPUT.
	Output* outputRecordingFrom{nullptr};
};
