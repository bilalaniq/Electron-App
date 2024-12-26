// Access DOM elements
const output = document.getElementById('output');
const inputField = document.getElementById('input');

// Add an event listener to detect when the user presses "Enter"
inputField.addEventListener('keypress', function(e) {
    if (e.key === 'Enter') {
        e.preventDefault(); // Prevent form submission

        const inputValue = inputField.value.trim();
        if (inputValue) {
            // Print the command entered
            printToTerminal(`> ${inputValue}`);

            // Process the command
            processCommand(inputValue);

            // Clear input field after execution
            inputField.value = '';
        }
    }
});

// Function to print to the terminal
function printToTerminal(text) {
    const pre = document.createElement('pre');
    pre.textContent = text;
    output.appendChild(pre);

    // Scroll to the bottom after printing new content
    output.scrollTop = output.scrollHeight;
}

// Process input commands using switch statement
function processCommand(command) {
    switch (command.toLowerCase()) {
        case 'help':
            printToTerminal('Available Commands:\nhelp - Show this help message\nclear - Clear the terminal\nabout - Show terminal info');
            break;
        case 'clear':
            output.innerHTML = ''; // Clears the terminal output
            break;
        case 'about':
            printToTerminal('Terminal version 1.0. Developed by ChatGPT.');
            break;
        case 'date':
            printToTerminal(`Current Date and Time: ${new Date().toLocaleString()}`);
            break;
        case 'hello':
            printToTerminal('Hello, welcome to the terminal!');
            break;
        default:
            printToTerminal(`Command not found: ${command}`);
            break;
    }
}
