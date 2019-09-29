import React, {Component, PureComponent} from 'react';
import {View, Image, Dimensions, StyleSheet, Text} from 'react-native';
import styled from 'styled-components';
import Slider from '@react-native-community/slider';

export default class BasicSettings extends PureComponent {
  constructor(props) {
    super(props);
    this.state = {brightnessValue: 0};
  }

  _onSlidingComplete = () => {
    console.log('Called on Sliding Complete');
    this.props.onBrightnessChange(this.state.brightnessValue);
  };

  //TODO: Only update brightness when value is changed
  render() {
    return (
      <SettingsView>
        <BrightnessText>
          Brightness: {this.state.brightnessValue}
        </BrightnessText>
        <Brightness
          minimumValue={0}
          maximumValue={100}
          minimumTrackTintColor="#FFFFFF"
          maximumTrackTintColor="#6E6E6E"
          thumbTintColor="#FFF"
          onValueChange={value => this.setState({brightnessValue: value})}
          onSlidingComplete={this._onSlidingComplete()}
          step={1}
          style={styles.brightnessStyle}
        />
      </SettingsView>
    );
  }
}

const SettingsView = styled.View``;

const BrightnessText = styled.Text`
  text-align: center;
  font-size: 22;
  color: #fff;
  font-family: 'Montserrat-Bold';
  margin-bottom: 6;
  margin-top: 10;
`;

const Brightness = styled.Slider``;

const styles = StyleSheet.create({
  brightnessStyle: {
    transform: [{scaleX: 2.0}, {scaleY: 2.0}],
    width: Dimensions.get('window').width / 2.0,
    alignSelf: 'center',
  },
});
