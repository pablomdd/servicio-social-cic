import React, { useState, useCallback, useEffect } from 'react';
import useWebSocket, { ReadyState, resetGlobalState } from 'react-use-websocket';
import { Box, VStack } from '@chakra-ui/react'
import AppBar from './components/AppBar';
import Controls from './components/Controls';

function App() {
  const [boardIpAddress, setBoardIpAddress] = useState<string | null>(null);
  const [isConnected, setIsConnected] = useState(false);
  const [isConnecting, setIsConnecting] = useState(false);

  const [messageHistory, setMessageHistory] = useState([{ data: "holi" }]);

  const { sendMessage, lastMessage, readyState } = useWebSocket(boardIpAddress);
  // const { sendMessage, lastMessage, readyState } = useWebSocket(socketUrl);

  useEffect(() => {
    if (lastMessage !== null) {
      setMessageHistory([...messageHistory, lastMessage]);
    }
  }, [lastMessage, setMessageHistory]);

  const handleClickSendMessage = useCallback(() => sendMessage('Hello'), []);

  const connectionStatus = {
    [ReadyState.CONNECTING]: 'Connecting',
    [ReadyState.OPEN]: 'Open',
    [ReadyState.CLOSING]: 'Closing',
    [ReadyState.CLOSED]: 'Closed',
    [ReadyState.UNINSTANTIATED]: 'Uninstantiated',
  }[readyState];

  // TODO: Clean unused functionality and state vars
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
    setBoardIpAddress(null);
    // if (isConnected) {
    //   console.log("disconnected");
    //   setIsConnected(false);
    // }
  }

  const wsSendMessage = (message: string) => {
    sendMessage(message);
  }

  return (
    <>
      <Box bg="white">
        <AppBar
          boardIpAddress={boardIpAddress}
          setBoardIpAddress={setBoardIpAddress}
          isConnected={isConnected}
          isConnecting={isConnecting}
          wsConnect={wsConnect}
          wsDisconnect={wsDisconnect}
          connectionStatus={connectionStatus}
        />
        <Controls
          wsSendMessage={wsSendMessage}
        />
      </Box>
      <footer>
        <VStack p="1em" color={"black"}>
          <button
            onClick={handleClickSendMessage}
            disabled={readyState !== ReadyState.OPEN}
          >
            Click Me to send 'Hello'
          </button>
          <span>The WebSocket is currently {connectionStatus}</span>
          {lastMessage ? <span>Last message: {lastMessage.data}</span> : null}
          <ul>
            {messageHistory.map((message, idx) => (
              <span key={idx}>{message ? message.data : null}</span>
            ))}
          </ul>
        </VStack>
      </footer>
    </>
  );
}
export default App;
