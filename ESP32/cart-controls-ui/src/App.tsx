import React, { useState } from 'react';
import { Box } from '@chakra-ui/react'
import AppBar from './components/AppBar';
import Controls from './components/Controls';

function App() {
  const [boardIpAddress, setBoardIpAddress] = useState("192.168.1.109");
  const [isConnected, setIsConnected] = useState(false);
  const [isConnecting, setIsConnecting] = useState(false);

  // TODO: Add functionality
  const wsConnect = () => {
    setIsConnecting(true);
    try {
      // mock functionality connecting to ws server
      setTimeout(() => {
        console.log("conected");
        setIsConnected(true);
        setIsConnecting(false);
      }, 1000);
    } catch {
      setIsConnecting(false);
      setIsConnected(false);
    }
  }

  // TODO: Add functionality
  const wsDisconnect = () => {
    if (isConnected) {
      console.log("disconnected");
      setIsConnected(false);
    }
  }

  return (
    <Box bg="white">
      <AppBar
        boardIpAddress={boardIpAddress}
        setBoardIpAddress={setBoardIpAddress}
        isConnected={isConnected}
        isConnecting={isConnecting}
        wsConnect={wsConnect}
        wsDisconnect={wsDisconnect}
      />
      <Controls />
    </Box>
  );
}
export default App;
