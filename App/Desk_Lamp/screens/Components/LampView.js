import React, {PureComponent} from 'react';
import {Text, View} from 'react-native';
import {connect} from 'react-redux';
import styled from 'styled-components';
import Svg, {Circle} from 'react-native-svg';
export class LampView extends PureComponent {
  constructor() {
    super();
    this.state = {
      lampState: [],
    };
  }

  rgbToHex = rgb => {
    var hex = Number(rgb).toString(16);
    if (hex.length < 2) {
      hex = '0' + hex;
    }
    return hex;
  };

  fullColorHex = (r, g, b) => {
    var red = this.rgbToHex(r);
    var green = this.rgbToHex(g);
    var blue = this.rgbToHex(b);
    return '#' + red + green + blue;
  };

  componentDidUpdate() {}

  componentDidMount() {
    let fillCount = 29;
    let startingArray = new Array(29).fill(['rgb(255,255,255)']);
    console.log(startingArray);
    this.setState({
      lampState: startingArray,
    });
  }

  render() {
    return (
      <Circle
        cx="50"
        cy="50"
        r="45"
        stroke="blue"
        strokeWidth="2.5"
        fill="green"
      />
    );
  }
}
const Background = styled.View`
  background-color: #000;
  flex: 1;
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
  {},
)(LampView);
