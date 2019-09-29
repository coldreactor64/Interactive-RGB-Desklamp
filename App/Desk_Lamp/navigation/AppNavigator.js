import Home from '../screens/Home';
import {createAppContainer} from 'react-navigation';
import {createStackNavigator} from 'react-navigation-stack';

const AppNavigator = createStackNavigator({
  homeScreen: {
    screen: Home,
    navigationOptions: {
      title: 'Interactive Desk lamp',
      header: null,
      headerBackTitle: 'Home',
    },
  },
});

export default createAppContainer(AppNavigator);
