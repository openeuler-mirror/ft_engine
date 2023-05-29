/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import { AsyncCallback, Callback } from './basic';

/**
 * Interface of display manager.
 * @syscap SystemCapability.WindowManager.WindowManager.Core
 * @since 7
 */
declare namespace display {
  /**
   * display error code
   * @since 7
   * @deprecated since 9
   */
   enum DMError {
    DM_ERROR_INIT_DMS_PROXY_LOCKED,
    DM_ERROR_IPC_FAILED,
    DM_ERROR_REMOTE_CREATE_FAILED,
    DM_ERROR_NULLPTR,
    DM_ERROR_INVALID_PARAM,
    DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED,
    DM_ERROR_DEATH_RECIPIENT,
    DM_ERROR_INVALID_MODE_ID,
    DM_ERROR_WRITE_DATA_FAILED,
    DM_ERROR_RENDER_SERVICE_FAILED,
    DM_ERROR_UNREGISTER_AGENT_FAILED,
    DM_ERROR_INVALID_CALLING,
    DM_ERROR_UNKNOWN,
  }

  /**
   * display error code
   * @since 9
   */
   enum DmErrorCode {
    DM_ERROR_NO_PERMISSION,
    DM_ERROR_INVALID_PARAM,
    DM_ERROR_DEVICE_NOT_SUPPORT,
    DM_ERROR_INVALID_SCREEN,
    DM_ERROR_SYSTEM_INNORMAL,
    DM_ERROR_INVALID_CALLING,
  }

  /**
   * Obtain the default display.
   * @since 7
   * @deprecated since 9, please use getDefaultDisplaySync instead.
   */
  function getDefaultDisplay(callback: AsyncCallback<Display>): void;

  /**
   * Obtain the default display.
   * @since 7
   * @deprecated since 9, please use getDefaultDisplaySync instead.
   */
  function getDefaultDisplay(): Promise<Display>;

  /**
   * Obtain the default display.
   * @since 9
   */
  function getDefaultDisplaySync(): Display;

  /**
   * Obtain all displays.
   * @since 7
   * @deprecated since 9, please use getAllDisplays instead.
   */
  function getAllDisplay(callback: AsyncCallback<Array<Display>>): void;

  /**
   * Obtain all displays.
   * @since 7
   * @deprecated since 9, please use getAllDisplays instead.
   */
  function getAllDisplay(): Promise<Array<Display>>;

  /**
   * Obtain all displays.
   * @since 9
   */
   function getAllDisplays(callback: AsyncCallback<Array<Display>>): void;

   /**
    * Obtain all displays.
    * @since 9
    */
   function getAllDisplays(): Promise<Array<Display>>;

  /**
   * Check whether there is a privacy window on the current display.
   * @param displayId Display id to query
   * @throws DM_ERROR_INVALID_PARAM If param is not valid
   * @systemapi Hide this for inner system use.
   * @since 9
   */
  function hasPrivateWindow(displayId: number): boolean;

  /**
   * Register the callback for display changes.
   * @param type: type of callback
   * @since 7
   * @throws DM_ERROR_INVALID_PARAM If param is not valid
   */
  function on(type: 'add' | 'remove' | 'change', callback: Callback<number>): void;

  /**
   * Unregister the callback for display changes.
   * @param type: type of callback
   * @since 7
   * @throws DM_ERROR_INVALID_PARAM If param is not valid
   */
  function off(type: 'add' | 'remove' | 'change', callback?: Callback<number>): void;

  /**
   * Enumerates the display states.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 7
   */
  enum DisplayState {
    /**
     * Unknown.
     */
    STATE_UNKNOWN = 0,
    /**
     * Screen off.
     */
    STATE_OFF,
    /**
     * Screen on.
     */
    STATE_ON,
    /**
     * Doze, but it will update for some important system messages.
     */
    STATE_DOZE,
    /**
     * Doze and not update.
     */
    STATE_DOZE_SUSPEND,
    /**
     * VR node.
     */
    STATE_VR,
    /**
     * Screen on and not update.
     */
    STATE_ON_SUSPEND,
  }

  /**
   * Rectangle
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 9
   */
  interface Rect {
    left: number;
    top: number;
    width: number;
    height: number;
  }

  /**
   * Curved area rects of the waterfall display.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 9
   */
  interface WaterfallDisplayAreaRects {
    readonly left: Rect;
    readonly right: Rect;
    readonly top: Rect;
    readonly bottom: Rect;
  }

  /**
   * cutout information of the display.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 9
   */
  interface CutoutInfo {
    /**
     * Bounding rectangles of the cutout areas of the display.
     */
    readonly boundingRects: Array<Rect>;

    /**
     * Rectangles of curved parts on each side of a waterfall display. 
     */
    readonly waterfallDisplayAreaRects: WaterfallDisplayAreaRects;
  }

  /**
   * Define properties of the display. They cannot be updated automatically.
   * @syscap SystemCapability.WindowManager.WindowManager.Core
   * @since 7
   */
  interface Display {
    /**
     * Display ID.
     */
    id: number;

    /**
     * Display name.
     */
    name: string;

    /**
     * The display is alive.
     */
    alive: boolean;

    /**
     * The state of display.
     */
    state: DisplayState;

    /**
     * Refresh rate, in Hz.
     */
    refreshRate: number;

    /**
     * Rotation degrees of the display.
     */
    rotation: number;

    /**
     * Display width, in pixels.
     */
    width: number;

    /**
     * Display height, in pixels.
     */
    height: number;

    /**
     * Display resolution.
     */
    densityDPI: number;

    /**
     * Display density, in pixels. The value for a low-resolution display is 1.0.
     */
    densityPixels: number;

    /**
     * Text scale density of the display.
     */
    scaledDensity: number;

    /**
     * DPI on the x-axis.
     */
    xDPI: number;

    /**
     * DPI on the y-axis.
     */
    yDPI: number;

    /**
     * Obtain the cutout info of the display.
     * @since 9
     */
    getCutoutInfo(callback: AsyncCallback<CutoutInfo>): void;

    /**
     * Obtain the cutout info of the display.
     * @since 9
     */
    getCutoutInfo(): Promise<CutoutInfo>;
  }
}

export default display;