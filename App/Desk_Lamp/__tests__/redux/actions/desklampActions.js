/* eslint-disable prettier/prettier */
import axios from 'axios';
import MockAdapter from 'axios-mock-adapter';
import {storeFactory} from '../../helpers/helperFunctions';
import {
  toggleLamp,
  getLampState,
  getLampIP,
  setBrightness,
} from '../../../redux/actions/desklampActions';

describe('toggleLamp', () => {
  test('Updates store correctly when returned 200 with data', () => {
    const store = storeFactory();
    var mock = new MockAdapter(axios);
    var data = {
      power: 1,
    };
    mock.onAny('http://desklamp.local/power?value=1').reply(200);
    return store.dispatch(toggleLamp(1, 'desklamp.local')).then(() => {
      const newState = store.getState();
      expect(newState.desklamp.Active).toEqual(1);
    });
  });

  test('Updates store correctly when returned 200 with no data', () => {});
  test('Updates store correctly when returned 404 with no data', () => {});
});
