import React, {Component, PureComponent} from 'react';
import {View, Image} from 'react-native';
import styled from 'styled-components';

export default class LampButton extends Component {
  constructor(props) {
    super(props);
    this.state = {
      startPress: null,
      delay: 400,
    };
  }

  _onStartShouldSetResponder = evt => {
    if (evt.nativeEvent.touches.length === 1) {
      this.state.startPress = Date.now();
      return true;
    }
    return false;
  };

  _onResponderRelease = () => {
    const now = Date.now();
    if (now - this.state.startPress > this.state.delay) {
      this.props.onLongPress();
    } else {
      this.props.onPress();
    }
    this.state.startPress = null;
  };

  render() {
    return (
      <Touch
        onStartShouldSetResponder={this._onStartShouldSetResponder}
        onResponderRelease={this._onResponderRelease}>
        {this.props.lampEnabled ? (
          <LampOn source={require('../../assets/images/DesklampOn.png')} />
        ) : (
          <LampOff source={require('../../assets/images/DesklampOff.png')} />
        )}
      </Touch>
    );
  }
}

const Touch = styled.View``;

const LampOn = styled.Image`
  width: 300px;
  height: 300px;
`;

const LampOff = styled.Image`
  width: 300px;
  height: 300px;
`;
