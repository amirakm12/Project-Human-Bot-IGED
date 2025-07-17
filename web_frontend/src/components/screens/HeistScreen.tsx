import React, { useState } from 'react';
import { Box, Button, Text, Textarea, VStack, Heading } from '@chakra-ui/react';

const HeistScreen: React.FC = () => {
  const [consoleOutput, setConsoleOutput] = useState('[Digital Heist Console]\nTargets: Facebook, Google, Microsoft, Apple, IBM\nCommands:\n- infiltrate [target]\n- extract data from [target]\n- weaponize exploits for [target]\n- gain control of [target]\n- erase traces from [target]');
  const [input, setInput] = useState('');

  const handleCommand = () => {
    // Simulate command execution
    let response = `Executed: ${input}`;
    // TODO: Integrate with backend
    setConsoleOutput(prev => `${prev}\n> ${input}\n${response}`);
    setInput('');
  };

  return (
    <Box p={4} bg="#0A0A0A" color="#00FFFF" fontFamily="Orbitron" height="100vh">
      <Heading mb={4} color="#00BFFF">Digital Heist Console</Heading>
      <Textarea
        value={consoleOutput}
        readOnly
        height="60%"
        bg="#1A1A1A"
        borderColor="#8A2BE2"
        fontFamily="JetBrains Mono"
      />
      <VStack mt={4} spacing={2}>
        <Textarea
          value={input}
          onChange={(e) => setInput(e.target.value)}
          placeholder="Enter command..."
          bg="#1A1A1A"
          borderColor="#00FFFF"
        />
        <Button onClick={handleCommand} bg="#8A2BE2" color="white">Execute</Button>
      </VStack>
    </Box>
  );
};

export default HeistScreen;