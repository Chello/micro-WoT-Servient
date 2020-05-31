const fs = require('fs');
var term = {};

window.onload = function() {

    JSONEditor.defaults.callbacks.template = {
        "filterFun":(jseditor, e) => {
            console.log(jseditor)
            console.log(e)
            return e.item.text
        },
    }

    editor = new JSONEditor(document.getElementById('editor_holder'),{
        // Enable fetching schemas via ajax
        ajax: true,

        //startval: initial,
        
        // The schema for the editor
        schema: {
            type: "object",
            $ref: "schemas/thing_desc.json",
            //$ref: "embeddedWoTServient/thing-schema.json"
        },
        //are fields all required? no
        required_by_default: false,
        //show checkbox for non-required opt
        show_opt_in: true,
        //show errors in editor
        show_errors: "change",
        //set theme
        theme: 'bootstrap4',
        //set icon library
        iconlib: "fontawesome4"
    });

    /*test = new JSONEditor(document.getElementById('test_holder'),{
        // Enable fetching schemas via ajax
        ajax: true,
        
        // The schema for the editor
        schema: {
            type: "object",
            $ref: "schemas/test.json",
            //$ref: "embeddedWoTServient/thing-schema.json"
        },
        //are fields all required? no
        required_by_default: false,
        //show checkbox for non-required opt
        show_opt_in: true,
        //show errors in editor
        show_errors: "change",
        //set theme
        theme: 'bootstrap4',
        //set icon library
        iconlib: "fontawesome4"
    });*/

    builder = new JSONEditor(document.getElementById('build_holder'),{
        // Enable fetching schemas via ajax
        ajax: true,
        
        // The schema for the editor
        schema: {
            type: "object",
            $ref: "schemas/build.json",
        },
        
        // Require all properties by default
        required_by_default: false,
        //show checkbox for non-required opt
        show_opt_in: true,
        //set theme
        theme: 'bootstrap4',
        //set icon library
        iconlib: "fontawesome4"
    });

    term = new CustomTerminal();
};
    
var composeTD = function() {
    //clone array
    //td = editor.getValue().map((x) => x);
    td = Object.assign({}, editor.getValue());
    options = Object.assign({}, builder.getValue());
    //properties arr becomes object of objects

    if (td["properties"]) {
        var arr = td['properties'];
        td['properties'] = {};
        arr.forEach(element => {
            var name = element.propertyName
            td['properties'][name] = element;
        });
    }

    //actions arr becomes object of objects
    if (td["actions"]) {
        var arr = td['actions'];
        td['actions'] = {};

        if (arr.length != 0) {
            options.actionFunctions = [];
        }

        arr.forEach(element => { //foreach action provided
            var name = element.actionName
            td['actions'][name] = element;

            //create array actionFunctions
            var actionCurrentFunction = {
                "source": 'cli',
                "name": name,
                "body": td['actions'][name]["body"],
                "output": {},
                "input": []
            };
            //if output is set
            if (td['actions'][name]['output']) {
                actionCurrentFunction.output = {
                    "name": '',
                    "type": td['actions'][name]['output']['type']
                }
            }
            //if inputs are set
            if (td['actions'][name]['input']) {
                var arr = td['actions'][name]['input'];
                td['actions'][name]['input'] = {};
                
                arr.forEach(element => { //foreach input provided
                    var inputName = element.inputName
                    td['actions'][name]['input'][inputName] = element;
                    //insert in options
                    actionCurrentFunction['input'].push({
                        "name": element.inputName,
                        "type": element.type
                    });
                });
            }

            options.actionFunctions.push(actionCurrentFunction);
        });
    }
    
    //events arr becomes object of objects
    if (td['events']) {
        var arr = td['events'];
        td['events'] = {};

        if (arr.length != 0) {
            options.eventConditions = [];
        }

        arr.forEach(element => { //foreach action provided
            var name = element.eventName
            td['events'][name] = element;

            eventCurrentCondition = {
                "condition": element.condition,
                "actions": element.actionsTriggered
            }

            options.eventConditions.push(eventCurrentCondition);
        });
    }
    return [td, options];

}

$('#submit_button').on('click', function() {

    console.log(editor.validate() /*? "true": "false"*/)
    console.log(builder.validate() /*? "true": "false"*/)

    // thing_prop = composeTD();
    // build_prop = builder.getValue();
    var [thing_prop, build_prop] = composeTD();
    console.log(thing_prop)
    // console.log(editor.getValue());
    // console.log(builder.getValue());

    var thingName = thing_prop['title'].toLowerCase();
    console.log(thingName)

    //create dir
    fs.mkdir(path.join(__dirname, thingName), (err) => { 
        if (err) { 
            return console.error(err); 
        } 
        console.log('Directory created successfully!'); 
    }); 
    //generate file names
    var tdFile = path.join(__dirname, thingName + '/' + thingName + '.json');
    var optFile = path.join(__dirname, thingName + '/opt_' + thingName + '.json');
    var tmplFile =  $('#board').val()
    //write td file into dir
    fs.writeFile(tdFile, 
                JSON.stringify(thing_prop), 
                function(err) {
                    if (err) { 
                        return console.error(err); 
                    } 
                    console.log('File ${thingName}.json created successfully!'); 
                })
    //write builder option file into dir
    fs.writeFile(optFile, 
                JSON.stringify(build_prop), 
                function(err) {
                    if (err) { 
                        return console.error(err); 
                    } 
                    console.log('File ${thingName}.json created successfully!'); 
                })
    //generate exec string
    var exec_str =  'embeddedWoTServient/embeddedWoTServient.py build' +
                    ' -T ' + tdFile + 
                    ' -o ' + optFile +
                    ' -t ' + tmplFile;
    term.exec(exec_str)
});
