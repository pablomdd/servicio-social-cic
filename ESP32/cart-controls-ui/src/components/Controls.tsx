import React from 'react';
import { useColorModeValue, useColorMode, Center, Box, SimpleGrid, Kbd, Button } from '@chakra-ui/react'

type ControlsProps = {
    wsSendMessage: Function,
};

export default function Controls({ wsSendMessage, }: ControlsProps) {

    const onDirectionKey = (direction: string) => {
        console.log(direction);
        wsSendMessage(direction);
    }

    return (
        <SimpleGrid p={{ base: "10%", lg: "12%" }} columns={3} spacing={10} >
            <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("UP")}>
                <Kbd fontSize={"2em"}>⬆</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("LF")}>
                <Kbd fontSize={"2em"}>⬅</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("RG")}>
                <Kbd fontSize={"2em"}>➡</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("DW")}>
                <Kbd fontSize={"2em"}>⬇</Kbd>
            </Button>
        </SimpleGrid>
    )
}
