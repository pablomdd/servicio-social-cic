import React from 'react';
import { useColorModeValue, useColorMode, Center, Box, SimpleGrid, Kbd, Button } from '@chakra-ui/react'

export default function Controls() {
    return (

        <SimpleGrid h="100vh" w="100vw" p={{ base: "10%", lg: "15%" }} columns={3} spacing={10} >
            <Button colorScheme='blue' height='80px'>
                <Kbd fontSize={"2em"}>⬆</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px'>
                <Kbd fontSize={"2em"}>⬅</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px'>
                <Kbd fontSize={"2em"}>➡</Kbd>
            </Button>
            <Button colorScheme='blue' height='80px'>
                <Kbd fontSize={"2em"}>⬇</Kbd>
            </Button>

        </SimpleGrid>

    )
}
