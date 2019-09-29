import axios from 'axios';
import {TOGGLE, SET_BRIGHTNESS, GET_STATUS, GET_IP} from './actionTypes';
//TODO: Add Error handling where it'll bring me back to connection page if theres an error

/**
 * @function toggleAxios
 * @param {*} active - 1 or 0 boolean values to turn off or on the lamp
 * @param {*} IP - IP from the desklamp from the redux store
 * @returns {boolean} - 1 or 0
 */
export const toggleAxios = async (active, IP) => {
  try {
    var lampValue = active ? 1 : 0;
    console.log(lampValue);
    const toggleLight = await axios.post(`
      http://${IP}/power?value=${lampValue}
      `);
    if (toggleLight.status === 200) {
      if (toggleLight.data.power !== undefined) {
        return toggleLight.data.power;
      } else {
        return 0;
      }
    } else {
      //Any other status return 0
      console.log(toggleLight.status);
      return 0;
    }
  } catch (error) {
    console.log(error);
    return 0;
  }
};

/**
 * @function toggleLamp - Toggles Lamp on or off
 * @param {*} active
 * @param {*} IP
 * @returns {dispatch} - dispatches state to store
 */
export const toggleLamp = (active, IP) => async dispatch => {
  const activeAxios = await toggleAxios(active, IP);
  dispatch({
    type: TOGGLE,
    Active: activeAxios,
  });
};

/**
 * @function getLampStateAxios - Grabs global state from Desklamp
 * @param {*} IP
 * @returns {JSON} - returns global state
 */
export const getLampStateAxios = async IP => {
  try {
    const LampStateAPI = axios.create({
      baseURL: 'http://192.168.1.5',
    });

    const toggleLight = await LampStateAPI.get('/all');

    if (toggleLight.status === 200) {
      return {
        Active: toggleLight.data.power,
        Brightness: toggleLight.data.brightness,
      };
    }
  } catch (error) {
    console.log(error);
    return {
      Active: 0,
      Brightness: 0,
    };
  }
};

/**
 * @function getLampState
 * @param {*} IP - IP of Desk Lamp
 * @returns {dispatch} - Dispatch updated state to Store
 */

export const getLampState = IP => async dispatch => {
  let lampState = await getLampStateAxios();
  console.log(lampState);
  dispatch({
    type: GET_STATUS,
    Active: lampState.Active,
    Brightness: lampState.Brightness,
  });
};

/**
 * @function getLampIPAxios - Get the lamp's IP address using its MDNS Address
 * @returns {String} - Returns String IP of the Desk Lamp
 */
export const getLampIPAxios = async () => {
  try {
    const API = axios.create({
      baseURL: 'http://192.168.1.5',
      timeout: 10000,
    });

    const toggleLight = await API.get('/ip');

    if (toggleLight.status === 200) {
      if (toggleLight.data.ip !== undefined) {
        return {
          IP: toggleLight.data.ip,
        };
      } else {
        return {
          IP: 'http://Desklamp.local',
        };
      }
    }
  } catch (error) {
    console.log(error);
    return {
      IP: 'http://Desklamp.local',
    };
  }
};

/**
 * @function getLampIP - Get the lamp's IP address using its MDNS
 * @returns {dispatch} - Dispatches IP up to global store.
 */
export const getLampIP = () => async dispatch => {
  let lampIP = await getLampIPAxios();
  console.log(lampIP);
  dispatch({
    type: GET_IP,
    IP: lampIP.IP,
  });
};

/**
 * @function setBrightnessAxios
 * @param {*} brightness - Brightness Value wanted
 * @param {*} IP - IP from the desklamp from the redux store
 * @returns {Number} - current Brightness
 */
export const setBrightnessAxios = async (brightness, IP) => {
  try {
    const toggleLight = await axios.post(`
      http://${IP}/brightness?value=${brightness}
      `);
    if (toggleLight.status === 200) {
      if (toggleLight.data.brightness !== undefined) {
        return toggleLight.data.brightness;
      } else {
        return 0;
      }
    } else {
      //Any other status return 0
      return 0;
    }
  } catch (error) {
    console.log(error);
    return 0;
  }
};

/**
 * @function setBrightness - Toggles Lamp on or off
 * @param {*} brightness
 * @param {*} IP
 * @returns {dispatch} - dispatches state to store
 */
export const setBrightness = (brightness, IP) => async dispatch => {
  console.log('Called Brightness');
  const Brightness = await setBrightnessAxios(brightness, IP);
  dispatch({
    type: SET_BRIGHTNESS,
    Brightness: Brightness,
  });
};
