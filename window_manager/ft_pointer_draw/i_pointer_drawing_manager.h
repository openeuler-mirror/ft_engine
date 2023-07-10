/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef I_POINTER_DRAWING_MANAGER_H
#define I_POINTER_DRAWING_MANAGER_H

#include <memory>
#include <vector>
#include <string>

enum Direction {
    Direction0,  // Rotating the display clockwise by 0 degree
    Direction90, // Rotating the display clockwise by 90 degrees
    Direction180, // Rotating the display clockwise by 180 degrees
    Direction270, // Rotating the display clockwise by 270 degrees
};

struct Rect {
    int32_t x; // X coordinate of the upper left corner
    int32_t y; // Y coordinate of the upper left corner
    int32_t width;
    int32_t height;
};

struct WindowInfo {
    static constexpr int32_t MAX_HOTAREA_COUNT = 10; // Maximum number of hot areas
    static constexpr uint32_t FLAG_BIT_UNTOUCHABLE = 1; // Untouchable window
    int32_t id; // Globally unique identifier of the window
    int32_t pid; // PID of the process where the window is located
    int32_t uid; // UID of the process where the window is located
    Rect area; // Window display area

    /**
     * Number of touch response areas (excluding the mouse response areas) in the window.
     * The value cannot exceed the value of MAX_HOTAREA_COUNT.
     */
    std::vector<Rect> defaultHotAreas;

    /**
     * Number of mouse response areas in the window. The value cannot exceed the value of MAX_HOTAREA_COUNT.
     */
    std::vector<Rect> pointerHotAreas;

    /**
     * Agent window ID
     */
    int32_t agentWindowId;

    /**
     * A 32-bit flag that represents the window status. If the 0th bit is 1,
     * the window is untouchable; if the 0th bit is 0, the window is touchable.
     */
    uint32_t flags;
};

/**
 * Physical screen information
 */
struct ScreenDisplayInfo {
    int32_t id; // Unique ID of the physical display
    int32_t x; // X coordinate of the upper left corner on the logical screen
    int32_t y; // Y coordinate of the upper left corner on the logical screen

    /**
     * Display width, which is the logical width of the original screen when the rotation angle is 0.
     * The value remains unchanged even if the display screen is rotated.
     */
    int32_t width;

    /**
     * Display height, which is the logical height of the original screen when the rotation angle is 0.
     * The value remains unchanged even if the display screen is rotated.
     */
    int32_t height;

    std::string name; // Name of the physical display, which is used for debugging
    Direction direction; // Orientation of the physical display

    /**
     * Unique screen ID, which is used to associate the corresponding touchscreen. The default value is default0.
     */
    std::string uniq;
};

/**
 * Logical screen information
 *
 * @since 9
 */
struct DisplayGroupInfo {
    int32_t width; // Width of the logical display
    int32_t height; // Height of the logical display
    int32_t focusWindowId; // ID of the focus window
    /**
     * List of window information of the logical display arranged in Z order, with the top window at the top
     */
    std::vector<WindowInfo> windowsInfo;
    std::vector<ScreenDisplayInfo> displaysInfo; // Physical screen information list
};

class IPointerDrawingManager {
public:
    IPointerDrawingManager() = default;
    virtual ~IPointerDrawingManager() = default;
    static std::shared_ptr<IPointerDrawingManager> GetInstance();
    virtual void DrawPointer(int32_t displayId, int32_t physicalX, int32_t physicalY, int32_t mouseStyle) {}
    virtual void UpdateDisplayInfo(const ScreenDisplayInfo& displayInfo) {}
    virtual void OnDisplayInfo(const DisplayGroupInfo& displayGroupInfo) {}
    virtual void OnWindowInfo(int32_t windowPid, int32_t windowId) {}
    virtual bool Init() { return true;}
    virtual void DeletePointerVisible(int32_t pid) {}
    virtual int32_t SetPointerVisible(int32_t pid, bool visible) { return 0;}
    virtual int32_t SetPointerStyle(int32_t pid, int32_t windowId, int32_t pointerStyle) { return 0; }
    virtual int32_t GetPointerStyle(int32_t pid, int32_t windowId, int32_t &pointerStyle) { return 0;}
    virtual void DrawPointerStyle() {}
    virtual bool IsPointerVisible() { return false; }
    virtual void SetMouseDisplayState(bool state) {}
    virtual bool GetMouseDisplayState() const { return true; }
    virtual void SetPointerLocation(int32_t pid, int32_t x, int32_t y) {}
};
#endif // I_POINTER_DRAWING_MANAGER_H