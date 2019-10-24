import Home from '../screens/Home';
import Effects from '../screens/Effects';
import {createAppContainer} from 'react-navigation';
import {createStackNavigator} from 'react-navigation-stack';

const AppNavigator = createStackNavigator({
  homeScreen: {
    screen: Effects,
    navigationOptions: {
      title: 'Interactive Desk lamp',
      header: null,
      headerBackTitle: 'Home',
    },
  },
});

export default createAppContainer(AppNavigator);
