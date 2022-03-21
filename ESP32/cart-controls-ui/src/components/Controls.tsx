import React from 'react';
import { useColorModeValue, useColorMode, Center, Box, Link, chakra, Button, Flex, HStack, Spacer, SimpleGrid } from '@chakra-ui/react'

export default function Controls() {
    return (
        <Box h="100%" w="100%" p="10%" bg="gray.100">
            <SimpleGrid columns={3} spacingX='40px' spacingY='20px'>
                <Box bg='tomato' height='80px'></Box>
                <Box bg='tomato' height='80px'></Box>
                <Box bg='tomato' height='80px'></Box>
                <Box bg='tomato' height='80px'></Box>
                <Box bg='tomato' height='80px'></Box>
            </SimpleGrid>
        </Box>
    )
}
