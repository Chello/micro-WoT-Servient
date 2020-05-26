// This file is required by the index.html file and will
// be executed in the renderer process for that window.
// No Node.js APIs are available in this process because
// `nodeIntegration` is turned off. Use `preload.js` to
// selectively enable features needed in the rendering
// process.

//import { Terminal } from "xterm";

window.onload = function() {

    console.log($)
    
    //const element = document.getElementById('editor_holder');
    editor = new JSONEditor(document.getElementById('editor_holder'),{
        // Enable fetching schemas via ajax
        ajax: true,
        
        // The schema for the editor
        schema: {
            type: "object",
            $ref: "schemas/thing_desc.json",
        },
        
        // Seed the form with a starting value
        //startval: starting_value,
        
        // Disable additional properties
        //no_additional_properties: false,
        
        // Require all properties by default
        required_by_default: true,
        //show checkbox for non-required opt
        show_opt_in: true
    });

    builder = new JSONEditor(document.getElementById('build_holder'),{
        // Enable fetching schemas via ajax
        ajax: true,
        
        // The schema for the editor
        schema: {
            type: "object",
            $ref: "schemas/build.json",
        },
        
        // Seed the form with a starting value
        //startval: starting_value,
        
        // Disable additional properties
        //no_additional_properties: false,
        
        // Require all properties by default
        required_by_default: true,
        //show checkbox for non-required opt
        show_opt_in: true
    });

    new CustomTerminal();
};
    


$('#submit_button').on('click', function() {
    console.log(editor.getValue());
  });