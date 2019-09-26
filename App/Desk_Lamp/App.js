/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 * @flow
 */

import React from 'react';
import AppNavigator from './navigation/AppNavigator'

export default class App extends React.Component{
  constructor() {
    super();
    this.state = {};
  }

  render() {
    return (
      <AppNavigator/>
    );
  }
}
