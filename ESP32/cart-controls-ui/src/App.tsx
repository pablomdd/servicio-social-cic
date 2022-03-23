import React from 'react';
import { Box } from '@chakra-ui/react'
import AppBar from './components/AppBar';
import Controls from './components/Controls';

function App() {
  return (
    <Box bg="white">
      <AppBar />
      <Controls />
    </Box>
  );
}
export default App;
