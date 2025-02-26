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

#include "gui/views/timeline_view.h"
#include <cstdint>

class ClipNavigationTimelineView : public TimelineView {
public:
	ClipNavigationTimelineView() = default;
	void focusRegained() override;
	ActionResult horizontalEncoderAction(int32_t offset) override;
	const char* getName() { return "clip_navigation_timeline_view"; }

protected:
	void horizontalScrollForLinearRecording(int32_t newXScroll);

	static int32_t xScrollBeforeFollowingAutoExtendingLinearRecording; // -1 means none
};
