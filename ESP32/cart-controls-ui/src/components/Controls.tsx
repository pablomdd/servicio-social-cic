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

        <SimpleGrid h="100vh" w="100vw" p={{ base: "10%", lg: "15%" }} columns={3} spacing={10} >
            <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("up")}>
                <Kbd fontSize={"2em"}>⬆</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("lf")}>
                <Kbd fontSize={"2em"}>⬅</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("rg")}>
                <Kbd fontSize={"2em"}>➡</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("dw")}>
                <Kbd fontSize={"2em"}>⬇</Kbd>
            </Button>
        </SimpleGrid>

    )
}
