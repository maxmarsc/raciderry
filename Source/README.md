# Source code
The source code of the application is entirely C++

Basics of audio development are needed to understand this project.

## JUCE
We use the JUCE Framework (>= 6.0.1) as a base for the audio and midi engine.

Once JUCE downloaded, you can open the `.projucer` file to generate a makefile for the wanted architecture.

See : https://juce.com/

## Coding Style
This project uses the JUCE guidelines with a few  additions :
 - JUCE coding standards : https://juce.com/discover/stories/coding-standards
 - Member variables are prepended with the `m_` prefix
 - Use classic Doxygen documentation format like the following

 ```cpp
 /**
 * @brief Get the Parameter object corresponding to the given identifier
 * 
 * @param id The identifier look for
 */
ControllableParameter getParameter(const juce::Identifier& id);
 ```