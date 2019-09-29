import {
  TOGGLE,
  SET_BRIGHTNESS,
  GET_STATUS,
  GET_IP,
} from '../actions/actionTypes';

let initialState = {
  Active: 0,
  Brightness: 0,
  IP: 'http://desklamp.local',
};

export default (state = initialState, action) => {
  switch (action.type) {
    case TOGGLE:
      return {
        ...state,
        Active: action.Active,
      };

    case SET_BRIGHTNESS:
      return {
        ...state,
        Active: state.Active,
        IP: state.IP,
        Brightness: action.Brightness,
      };

    case GET_STATUS:
      return {
        ...state,
        Active: action.Active,
        Brightness: action.Brightness,
        IP: state.IP,
      };

    case GET_IP:
      return {
        ...state,
        IP: action.IP,
        Active: state.Active,
        Brightness: state.Brightness,
      };

    default:
      return state;
  }
};
