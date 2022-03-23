import React from 'react';
import { useColorModeValue, useColorMode, Box, Link, chakra, Button, Flex, HStack, Spacer, Spinner, Input, InputGroup, InputLeftAddon } from '@chakra-ui/react'

export default function AppBar() {
    const bg = useColorModeValue("white", "gray.100");
    const cl = useColorModeValue("gray.200", "white");
    const { toggleColorMode: toggleMode } = useColorMode();
    const text = useColorModeValue("dark", "light");
    // const SwitchIcon = useColorModeValue(FaMoon, FaSun);
    const ic = useColorModeValue("brand.600", "brand.50");
    const hbg = useColorModeValue("gray.50", "brand.400");
    const tcl = useColorModeValue("gray.900", "gray.50");
    const dcl = useColorModeValue("gray.500", "gray.50");
    const hbgh = useColorModeValue("gray.100", "brand.500");


    return (
        <React.Fragment>
            <chakra.header bg={bg} px={{ base: 2, sm: 4 }} py={4} shadow="lg">
                <Flex alignItems="center" justifyContent="space-between" mx="auto">
                    <Box display={{ base: "none", md: "inline-flex" }}>
                        <HStack spacing={1}>
                            <Box role="group">

                                <Button
                                    bg={bg}
                                    color="gray.500"
                                    alignItems="center"
                                    fontSize="md"
                                    _hover={{ color: cl }}
                                    _focus={{ boxShadow: "none" }}
                                >
                                    Cart Control UI
                                </Button>
                            </Box>
                        </HStack>
                    </Box>
                    {/* <Spacer /> */}
                    <HStack>
                        <InputGroup colorScheme={"whiteAlpha"}>
                            <InputLeftAddon children='ws://' bg={"white"} color={"black"} border="1px" />
                            <Input
                                placeholder='ip address'
                                bg={"white"}
                                color={"black"}
                                defaultValue="192.65.1.92"
                            />
                        </InputGroup>
                        <Button
                            isLoading={false}
                            colorScheme='green'
                            spinner={<Spinner size='md' />}
                        >
                            Conectar
                        </Button>
                    </HStack>
                    {/* <Box display="flex" alignItems="center">
                        <IconButton
                            size="md"
                            fontSize="lg"
                            aria-label={`Switch to ${text} mode`}
                            variant="ghost"
                            color="current"
                            ml={{ base: "0", md: "3" }}
                            onClick={toggleMode}
                            icon={<SwitchIcon />}
                        />
                        <IconButton
                            display={{ base: "flex", md: "none" }}
                            aria-label="Open menu"
                            fontSize="20px"
                            color={useColorModeValue("gray.800", "inherit")}
                            variant="ghost"
                            icon={<AiOutlineMenu />}
                            onClick={mobileNav.onOpen}
                        />
                    </Box> */}
                </Flex>
            </chakra.header>
        </React.Fragment>
    );
}
