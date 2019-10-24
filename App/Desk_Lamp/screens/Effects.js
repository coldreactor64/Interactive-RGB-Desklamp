import React, {useState, useEffect} from 'react';
import {Text, View, Dimensions} from 'react-native';
import {connect} from 'react-redux';
import styled from 'styled-components';
//import LampView from './Components/LampView';
import Svg, {Circle} from 'react-native-svg';

const ws = new WebSocket('ws://192.168.1.5:81/');

const Effects = () => {
  const [ledState, setLEDState] = useState({
    ledArray1: [
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
    ],
    ledArray2: [
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
    ],
    ledArray3: [
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
    ],
    ledArray4: [
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
    ],
    ledArray5: [
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
    ],
    ledArray6: [
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
    ],
    ledArray7: [
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
      ['rgb(255,255,255)'],
    ],
  });
  const screenWidth = Math.round(Dimensions.get('window').width) - 20;
  const [webSocketStatus, setWebSocketStatus] = useState(false);
  useEffect(() => {
    //on Mount
    console.log('Mounted');
    //On unmount
    return () => {
      ws.close();
    };
  }, []);

  useEffect(() => {}, [ledState]);
  useEffect(() => {
    if (webSocketStatus === false) {
    }
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [webSocketStatus]);

  ws.onopen = () => {
    // connection opened
    ws.send('on'); // turn on
    setWebSocketStatus(true);
  };

  ws.onmessage = e => {
    // a message was received
    const recievedJSON = JSON.parse(e.data);
    //console.log(recievedJSON.leds[0][0]);
    if (recievedJSON.leds[0][0] !== undefined) {
      console.log('running');
      let ledArray1 = [];
      let ledArray2 = [];
      let ledArray3 = [];
      let ledArray4 = [];
      let ledArray5 = [];
      let ledArray6 = [];
      let ledArray7 = [];

      for (let i = 0; i < 29; i++) {
        if (i <= 2) {
          ledArray1.push(
            `rgb(${recievedJSON.leds[i][0]}, ${recievedJSON.leds[i][1]}, ${
              recievedJSON.leds[i][2]
            })`,
          );
        } else if (i <= 6) {
          ledArray2.push(
            `rgb(${recievedJSON.leds[i][0]}, ${recievedJSON.leds[i][1]}, ${
              recievedJSON.leds[i][2]
            })`,
          );
        } else if (i <= 11) {
          ledArray3.push(
            `rgb(${recievedJSON.leds[i][0]}, ${recievedJSON.leds[i][1]}, ${
              recievedJSON.leds[i][2]
            })`,
          );
        } else if (i <= 16) {
          ledArray4.push(
            `rgb(${recievedJSON.leds[i][0]}, ${recievedJSON.leds[i][1]}, ${
              recievedJSON.leds[i][2]
            })`,
          );
        } else if (i <= 21) {
          ledArray5.push(
            `rgb(${recievedJSON.leds[i][0]}, ${recievedJSON.leds[i][1]}, ${
              recievedJSON.leds[i][2]
            })`,
          );
        } else if (i <= 25) {
          ledArray6.push(
            `rgb(${recievedJSON.leds[i][0]}, ${recievedJSON.leds[i][1]}, ${
              recievedJSON.leds[i][2]
            })`,
          );
        } else if (i <= 28) {
          ledArray7.push(
            `rgb(${recievedJSON.leds[i][0]}, ${recievedJSON.leds[i][1]}, ${
              recievedJSON.leds[i][2]
            })`,
          );
        } else {
          console.log('What else');
        }
      }
      setLEDState({
        ledArray1: ledArray1,
        ledArray2: ledArray2,
        ledArray3: ledArray3,
        ledArray4: ledArray4,
        ledArray5: ledArray5,
        ledArray6: ledArray6,
        ledArray7: ledArray7,
      });
      //console.log(ledState);
    }
    //console.log(e.data);
  };

  ws.onerror = e => {
    // an error occurred
    console.log('Error');
    console.log(e.message);
    setWebSocketStatus(false);
  };

  ws.onclose = e => {
    // connection closed
    console.log('Closed');
    console.log(e.code, e.reason);
    setWebSocketStatus(false);
  };

  return (
    <Background>
      <BackgroundCircle width={screenWidth}>
        <LampRow3 color={ledState.ledArray1} />
        <LampRow4 color={ledState.ledArray2} />
        <LampRow5 color={ledState.ledArray3} />
        <LampRow5 color={ledState.ledArray4} />
        <LampRow5 color={ledState.ledArray5} />
        <LampRow4 color={ledState.ledArray6} />
        <LampRow3 color={ledState.ledArray7} />
      </BackgroundCircle>
    </Background>
  );
};

const LampRow3 = ({color}) => {
  return (
    <RowView3>
      <StartPadding />
      <LED color={color[0]} />
      <LED color={color[1]} />
      <LED color={color[2]} />
      <EndPadding />
    </RowView3>
  );
};
const StartPadding = styled.View`
  margin-left: 10px;
`;
const EndPadding = styled.View`
  margin-right: 10px;
`;
const RowView3 = styled.View`
  flex-direction: row;
  justify-content: space-evenly;
  padding-left: 40;
  padding-right: 40;
  flex: 1;
`;

const LampRow4 = ({color}) => {
  return (
    <RowView4>
      <LED color={color[0]} />
      <LED color={color[1]} />
      <LED color={color[2]} />
      <LED color={color[3]} />
    </RowView4>
  );
};
const RowView4 = styled.View`
  flex-direction: row;
  justify-content: space-evenly;
  flex: 1;
`;

const LampRow5 = ({color}) => {
  return (
    <RowView5>
      <LED color={color[0]} />
      <LED color={color[1]} />
      <LED color={color[2]} />
      <LED color={color[3]} />
      <LED color={color[4]} />
    </RowView5>
  );
};

const RowView5 = styled.View`
  flex-direction: row;
  justify-content: space-evenly;
  flex: 1;
`;

const LampView = styled.View`
  align-items: center;
`;

const Background = styled.View`
  background-color: #000;
  flex: 1;
`;

const LED = styled.View`
  width: 36;
  height: 36;
  border-radius: ${40 / 2};
  background-color: ${props => props.color};
  align-self: center;
`;

const BackgroundCircle = styled.View`
  height: ${props => props.width};
  width: ${props => props.width};
  border-radius: ${props => props.width / 2};
  align-self: center;
  background-color: gray;
  margin-top: 100px;
  flex-direction: column;
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
)(Effects);
