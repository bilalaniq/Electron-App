# My First Electron App

This is my first Electron app! Electron is a framework for building cross-platform desktop apps with JavaScript, HTML, and CSS. This app serves as an example of how to create and configure a basic Electron app.

## Features

- Basic Electron setup
- Cross-platform support (Windows, macOS, Linux)
- Sample UI with HTML, CSS, and JavaScript
- Integration with Node.js APIs
- cpp(Addons)

## Prerequisites

Before running the app, make sure you have the following installed:

- [Node.js](https://nodejs.org/) (which includes npm)
- npm or yarn (yarn is more recummeeded)
- python
- cpp (node_api.h & other libraries)

## Installation

1. **Clone the repository**:

   If you haven't cloned the repository yet, run the following command:

   git clone https://github.com/bilalaniq/Electron-App.git

2. **Install dependencies**:

   you need to go to https://nodejs.org/en/download/source-code this website to donwload the node.js
   or could just do

   # download and install Node.js
   fnm install 22

   Navigate to your project directory and run:

   npm install
   yarn start
   npm list electron (to cheak if node_modules for elctron are installed)

   This will install all the dependencies, including Electron.

## Running the App

To start the app, run the following command in your terminal:


npm start
or 
yarn start

This will launch your Electron application, and you should see a basic window pop up with your app's UI.

## File Structure

Here’s an overview of the key files and directories in the project:

```
├── package.json           # Project metadata and dependencies
├── main.js                # Main process code (Electron entry point)
├── index.html             # HTML file for the app window
├── renderer.js            # Renderer process code (handles the UI)
├── node_modules/          # Node.js modules (installed dependencies)
├── .gitignore             # Files and directories to ignore in Git
└── README.md              # This file
```

- `main.js`: The main entry point for the Electron app. It initializes the app, creates the window, and manages application lifecycle events.
- `index.html`: The HTML file that serves as the UI for your Electron app.
- `renderer.js`: Handles the behavior of the HTML interface (renderer process).

## Contributing

If you'd like to contribute to this project, feel free to fork the repository and create a pull request with your changes. Please follow the best practices and maintain a clean code structure.