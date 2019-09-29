import desklampReducer from './desklampReducer';
import {combineReducers} from 'redux';

export default combineReducers({
  desklamp: desklampReducer,
});
