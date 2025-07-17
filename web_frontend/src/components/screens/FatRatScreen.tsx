import React, { useState } from 'react'; 
import { Box, Button, Text, Textarea, VStack, Heading } from '@chakra-ui/react'; 
 
const FatRatScreen: React.FC = () => { 
  const [consoleOutput, setConsoleOutput] = useState('[FatRat Console v4.2]\\nCommands:\\n > view full-screen\\n > toggle camera front/back\\n > spoof cam feed on/off\\n > capture screenshot\\n > record video [duration]\\nAgentZERO Status:\\n - Nodes connected: 0\\n - AR feed: Inactive\\n - Sync cameras: Disabled'); 
  const [input, setInput] = useState(''); 
 
  const handleCommand = () => { 
    // Simulate command execution 
    let response = `Executed: ${input}`; 
    // TODO: Integrate with backend API for actual command execution 
    setConsoleOutput(prev => `${prev}\\n> ${input}\\n${response}`); 
    setInput(''); 
  }; 
 
  return ( 
    <Box p={4} bg=\"#0A0A0A\" color=\"#00FFFF\" fontFamily=\"Orbitron\" height=\"100vh\"> 
      <Heading mb={4} color=\"#00BFFF\">FatRat Console</Heading> 
      <Textarea 
        value={consoleOutput} 
        readOnly 
        height=\"60%\" 
        bg=\"#1A1A1A\" 
        borderColor=\"#8A2BE2\" 
        fontFamily=\"JetBrains Mono\" 
      /> 
      <VStack mt={4} spacing={2}> 
        <Textarea 
          value={input} 
          onChange={(e) => setInput(e.target.value)} 
          placeholder=\"Enter command...\" 
          bg=\"#1A1A1A\" 
          borderColor=\"#00FFFF\" 
        /> 
        <Button onClick={handleCommand} bg=\"#8A2BE2\" color=\"white\">Execute</Button> 
      </VStack> 
    </Box> 
  ); 
}; 
 
export default FatRatScreen;