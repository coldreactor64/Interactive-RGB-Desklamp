import React, {PureComponent} from 'react';
import {Text, View} from 'react-native';
import {connect} from 'react-redux';
import styled from 'styled-components';
import LampButton from './Components/LampButton';
import BasicSettings from './Components/BasicSettings';

import {
  toggleLamp,
  getLampState,
  getLampIP,
  setBrightness,
} from '../redux/actions/desklampActions';

export class Home extends PureComponent {
  constructor() {
    super();
    this.state = {
      lampEnabled: 0,
    };
  }

  _onPressLamp = async () => {
    if (this.props.Active === 0) {
      console.log('Its off');
      await this.props.toggleLamp(1, this.props.IP);
      return null;
    }
    if (this.props.Active === 1) {
      console.log('its on');
      await this.props.toggleLamp(0, this.props.IP);
      return null;
    } else {
      console.log('This is different');
      console.log(this.props.Active);
    }
  };

  _onLongPressLamp = () => {
    console.log('Press two');
  };

  _onBrightnessChange = async newBrightness => {
    console.log('Called On BrightnessChange');
    await this.props.setBrightness(newBrightness, this.props.IP);
  };

  componentDidMount = async () => {
    await this.props.getLampIP();
    await this.props.getLampState();
  };

  render() {
    return (
      <Background>
        <Header>Interactive Lamp</Header>
        <LampToggle>
          <LampButton
            lampEnabled={this.props.Active}
            onPress={this._onPressLamp}
            onLongPress={this._onLongPressLamp}
          />
        </LampToggle>
        <SettingsView>
          <BasicSettings onBrightnessChange={this._onBrightnessChange} />
        </SettingsView>
      </Background>
    );
  }
}
const Background = styled.View`
  background-color: #000;
  flex: 1;
`;

const Header = styled.Text`
  text-align: center;
  font-size: 28;
  color: #fff;
  font-family: 'Montserrat-Bold';
  margin-bottom: 6;
  margin-top: 10;
`;

const LampToggle = styled.View`
  align-items: center;
  justify-content: center;
  flex: 1;
`;

const SettingsView = styled.View`
  margin-left: 8px;
  margin-right: 8px;
  margin-bottom: 10px;
  border-radius: 20px;
  align-content: center;
  justify-content: center;
  flex: 0.3;
  background-color: #000234;
`;

function mapStateToProps(state) {
  return {
    Active: state.desklamp.Active,
    Brightness: state.desklamp.Brightness,
    IP: state.desklamp.IP,
  };
}
export default connect(
  mapStateToProps,
  {toggleLamp, getLampState, getLampIP, setBrightness},
)(Home);
