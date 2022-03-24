import React, { useState } from 'react';
import { Box } from '@chakra-ui/react'
import AppBar from './components/AppBar';
import Controls from './components/Controls';

function App() {
  const [boardIpAddress, setBoardIpAddress] = useState("192.168.1.109");
  const [isConnected, setIsConnected] = useState(false);
  const [isConnecting, setIsConnecting] = useState(true);
  return (
    <Box bg="white">
      <AppBar
        boardIpAddress={boardIpAddress}
        setBoardIpAddress={setBoardIpAddress}
        isConnected={isConnected}
        isConnecting={isConnecting}
      />
      <Controls />
    </Box>
  );
}
export default App;
