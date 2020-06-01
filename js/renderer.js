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
    
var composeTD = function() {
    //clone array
    //td = editor.getValue().map((x) => x);
    // let td = Object.assign({}, editor.getValue());
    // let options = Object.assign({}, builder.getValue());
    //properties arr becomes object of objects

    let td = JSON.parse(JSON.stringify(editor.getValue()))
    let options = JSON.parse(JSON.stringify(builder.getValue()))


    //check if thing using websocket
    if (td["useWS"]) {
        td["forms"][1] = td["forms"][0]
    }

    if (td["properties"]) {
        var propArr = td['properties'];
        td['properties'] = {};
        
        propArr.forEach(element => {
            var name = element.propertyName
            td['properties'][name] = element;

            //check if property using websocket
            if (td["properties"][name]["useWS"]) {
                td["properties"][name]["forms"][1] = td["properties"][name]["forms"][0]
            }
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
                });
            }

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
                td["events"][name]["forms"][1] = td["events"][name]["forms"][0]
            }

            eventCurrentCondition = {
                "condition": element.condition,
                "actions": element.actionsTriggered
            }

            //if subscription are set
            if (td['events'][name]['subscription']) {
                var arrSubscription = td['events'][name]['subscription'];
                td['events'][name]['subscription'] = {};
                
                arrSubscription.forEach(element => { //foreach subscription provided
                    var subscriptionName = element.subscriptionName
                    td['events'][name]['subscription'][subscriptionName] = element;
                });
            }

            //if data are set
            if (td['events'][name]['data']) {
                var arrData = td['events'][name]['data'];
                td['events'][name]['data'] = {};
                
                arrData.forEach(element => { //foreach data provided
                    var dataName = element.dataName
                    td['events'][name]['data'][dataName] = element;
                });
            }

            //if cancellation are set
            if (td['events'][name]['cancellation']) {
                var arrCancellation = td['events'][name]['cancellation'];
                td['events'][name]['cancellation'] = {};
                
                arrCancellation.forEach(element => { //foreach cancellation provided
                    var cancellationName = element.cancellationName
                    td['events'][name]['cancellation'][cancellationName] = element;
                });
            }

            options.eventConditions.push(eventCurrentCondition);
        });
    }
    return [td, options];

}

$('#submit_button').on('click', function() {

    console.log(editor.validate() /*? "true": "false"*/)
    console.log(builder.validate() /*? "true": "false"*/)

    if (editor.validate().length != 0) {
        alert("Some errors found in form. Please check")
        return;
    }

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
