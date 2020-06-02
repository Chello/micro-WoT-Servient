const fs = require('fs');
var term = {};

window.onload = function() {
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

        object_layout: "table",
        //show checkbox for non-required opt
        show_opt_in: true,
        //show errors in editor
        show_errors: "always",
        //set theme
        theme: 'bootstrap4',
        //set icon library
        iconlib: "fontawesome4"
    });

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

/**
 * This function composes the json variables for both thing description and thing options.
 * This object must be saved and passed to embeddedWoTServient python script for building the .ino code
 */
var composeTD = function() {
    //clone array
    let td = JSON.parse(JSON.stringify(editor.getValue()))
    let options = JSON.parse(JSON.stringify(builder.getValue()))

    //check if thing using websocket
    if (td["useWS"]) {
        td["forms"][1] = td["forms"][0]
        //delete useWS 
        td["useWS"] = undefined;
    }

    if (td["properties"]) {
        var propArr = td['properties'];
        td['properties'] = {};
        
        propArr.forEach(element => {//foreach property
            var name = element.propertyName
            td['properties'][name] = element;

            //check if property using websocket
            if (td["properties"][name]["useWS"]) {
                td["properties"][name]["forms"][1] = td["properties"][name]["forms"][0]
                //delete useWS
                td["properties"][name]["useWS"] = undefined;
            }

            //delete propertyName 
            td['properties'][name]['propertyName'] = undefined;
        });
    }

    //actions arr becomes object of objects
    if (td["actions"]) {
        var actionArr = td['actions'];
        td['actions'] = {};

        //check if action using websocket
        
        if (actionArr.length != 0) {
            options.actionFunctions = [];
        }
        
        actionArr.forEach(element => { //foreach action provided
            var name = element.actionName
            td['actions'][name] = element;
            
            if (td["actions"][name]["useWS"]) {
                td["actions"][name]["forms"][1] = td["actions"][name]["forms"][0]
                //delete useWS
                td["actions"][name]["useWS"] = undefined;
            }

            //create array actionFunctions
            var actionCurrentFunction = {
                "source": 'cli',
                "name": name,
                "body": td['actions'][name]["body"],
                "output": {},
                "input": []
            };
            //if output is set
            if (element['output']) {
                actionCurrentFunction.output = {
                    "name": '',
                    "type": td['actions'][name]['output']['type']
                }
            }
            //if inputs are set
            if (element['input']) {
                var inputArr = td['actions'][name]['input'];
                td['actions'][name]['input'] = {};
                
                inputArr.forEach(element => { //foreach input provided
                    var inputName = element.inputName
                    td['actions'][name]['input'][inputName] = element;
                    //insert in options
                    actionCurrentFunction['input'].push({
                        "name": element.inputName,
                        "type": element.type
                    });

                    //delete inputName element
                    td['actions'][name]['input'][inputName]['inputName'] = undefined;
                });
            }
            //delete actionName field
            td['actions'][name]['actionName'] = undefined;
            //delete body field
            td['actions'][name]['body'] = undefined;

            options.actionFunctions.push(actionCurrentFunction);
        });
    }
    
    //events arr becomes object of objects
    if (td['events']) {
        var evtArr = td['events'];
        td['events'] = {};

        
        if (evtArr.length != 0) {
            options.eventConditions = [];
        }
        
        evtArr.forEach(element => { //foreach event provided
            var name = element.eventName
            td['events'][name] = element;
            //check if event using websocket
            if (td["events"][name]["useWS"]) {
                td["events"][name]["forms"][1] = td["events"][name]["forms"][0];
                //delete useWS
                td["events"][name]["useWS"] = undefined;
            }

            eventCurrentCondition = {
                "condition": element.condition,
                "actions": element.actionsTriggered
            }

            //if subscription is set
            if (td['events'][name]['subscription']) {
                var arrSubscription = td['events'][name]['subscription'];
                td['events'][name]['subscription'] = {};
                
                arrSubscription.forEach(element => { //foreach subscription provided
                    var subscriptionName = element.subscriptionName
                    td['events'][name]['subscription'][subscriptionName] = element;
                    //delete subscriptionName
                    td['events'][name]['subscription'][subscriptionName]['subscriptionName'] = undefined;
                });
            }

            //if data is set
            if (td['events'][name]['data']) {
                var arrData = td['events'][name]['data'];
                td['events'][name]['data'] = {};
                
                arrData.forEach(element => { //foreach data provided
                    var dataName = element.dataName
                    td['events'][name]['data'][dataName] = element;
                    //delete dataName
                    td['events'][name]['data'][dataName]['dataName'] = undefined;
                });
            }

            //if cancellation is set
            if (td['events'][name]['cancellation']) {
                var arrCancellation = td['events'][name]['cancellation'];
                td['events'][name]['cancellation'] = {};
                
                arrCancellation.forEach(element => { //foreach cancellation provided
                    var cancellationName = element.cancellationName
                    td['events'][name]['cancellation'][cancellationName] = element;
                    //delete cancellationName
                    td['events'][name]['cancellation'][cancellationName]['cancellationName'] = undefined;
                });
            }

            options.eventConditions.push(eventCurrentCondition);
        });
    }
    return [td, options];

}

/**
 * Action triggered when submit-button is clicked
 */
$('#submit_button').on('click', function() {
    console.log(editor.validate() /*? "true": "false"*/)
    console.log(builder.validate() /*? "true": "false"*/)

    if (editor.validate().length != 0) {
        alert("Some errors found in form. Please check")
        return;
    }

    // thing_prop = composeTD();
    // build_prop = builder.getValue();
    // var [thing_prop, build_prop] = composeTD();
    // console.log(thing_prop)
    // console.log(editor.getValue());
    // console.log(builder.getValue());

    var thingName = editor.getValue()['title'];
    console.log(thingName)

    //generate directory path name
    var dirPath = path.join(__dirname, thingName);
    //if does not exist directory
    if (!fs.existsSync(dirPath)) {
        //create it
        createThingFiles(dirPath, thingName);
    } else {
        //else ask if wanted to overwrite   
        if (confirm("Thing "+dirPath+" already exists! Do you want to overwrite it?")) {
            createThingFiles(dirPath, thingName);
        }
    }
    
});

/**
 * Creates thing directory. 
 * If creation success, then creates files
 * @param {String} dirPath the path that will contain files
 * @param {String} thingName the thing title
 */
var createThingFiles = function(dirPath, thingName) {
    fs.mkdir(dirPath, (err) => { 
        if (err && err.code != "EEXIST") { 
            return console.error(err); 
        } 
        console.log('Directory created successfully!'); 
        createTDFiles(thingName);
    }); 
}

/**
 * Creates thing files and then launches py builder
 * @param {String} thingName the name of the thing
 */
var createTDFiles = function(thingName) {
    //generate
    var [thing_prop, build_prop] = composeTD();
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
}