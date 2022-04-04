import React from 'react';
import {
    useColorModeValue, useColorMode, Center, Box,
    SimpleGrid,
    Kbd,
    Button,
    Flex,
    Slider,
    SliderMark,
    SliderTrack,
    SliderFilledTrack,
    SliderThumb,
    Text
} from '@chakra-ui/react'

type ControlsProps = {
    wsSendMessage: Function,
};

export default function Controls({ wsSendMessage, }: ControlsProps) {

    const [sliderValue, setSliderValue] = React.useState(50)

    const onDirectionKey = (direction: string) => {
        console.log(direction);
        wsSendMessage(direction);
    }

    const onSliderChangeEnd = (val: number) => {
        console.log("velocity", val);
        setSliderValue(val);
        wsSendMessage(val.toString());
    }

    return (
        <>
            <Flex px={{ base: "10%", lg: "12%" }} py="3em">
                <Text color={"black"} px="1em">
                    Velocidad
                </Text>
                <Slider
                    aria-label='velocity'
                    step={1}
                    min={0}
                    max={255}
                    onChangeEnd={(val) => onSliderChangeEnd(val)}
                >
                    <SliderMark value={25} mt='1' ml='-2.5' fontSize='sm'>
                        25%
                    </SliderMark>
                    <SliderMark value={50} mt='1' ml='-2.5' fontSize='sm'>
                        50%
                    </SliderMark>
                    <SliderMark value={75} mt='1' ml='-2.5' fontSize='sm'>
                        75%
                    </SliderMark>
                    <SliderMark
                        value={sliderValue}
                        textAlign='center'
                        bg='blue.500'
                        color='white'
                        mt='-10'
                        ml='-5'
                        w='12'
                    >
                        {sliderValue}
                    </SliderMark>
                    <SliderTrack>
                        <SliderFilledTrack />
                    </SliderTrack>
                    <SliderThumb />
                </Slider>
            </Flex>
            <SimpleGrid px={{ base: "10%", lg: "12%" }} py={{ base: "10%", lg: "5%" }} columns={3} spacing={10} >
                <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("FW")}>
                    <Kbd fontSize={"2em"}>⬆</Kbd>
                </Button>
                <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("LF")}>
                    <Kbd fontSize={"2em"}>⬅</Kbd>
                </Button>
                <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("RG")}>
                    <Kbd fontSize={"2em"}>➡</Kbd>
                </Button>
                <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("BC")}>
                    <Kbd fontSize={"2em"}>⬇</Kbd>
                </Button>
                <Button colorScheme='blue' height='80px' onClick={() => onDirectionKey("ST")}>
                    <Kbd fontSize={"2em"}>STOP</Kbd>
                </Button>
            </SimpleGrid>
        </>
    )
}
