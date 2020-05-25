
window.addEventListener('DOMContentLoaded', () => {
  const replaceText = (selector, text) => {
    const element = document.getElementById(selector)
    if (element) element.innerText = text
  }

  for (const type of ['chrome', 'node', 'electron']) {
    replaceText(`${type}-version`, process.versions[type])
  }

  //const {JSONEditor} = require('@json-editor/json-editor')
  console.log(JSONEditor);

  var starting_value = [
    {
      name: "John Smith",
      age: 35,
      gender: "male",
      location: {
        city: "San Francisco",
        state: "California",
        citystate: ""
      },
      pets: [
        {
          name: "Spot",
          type: "dog",
          fixed: true
        },
        {
          name: "Whiskers",
          type: "cat",
          fixed: false
        }
      ]
    }
  ];
  
  //const element = document.getElementById('editor_holder');
  var editor = new JSONEditor(document.getElementById('editor_holder'),{
    // Enable fetching schemas via ajax
    ajax: true,
    
    // The schema for the editor
    schema: {
      type: "object",
      // title: "People",
      // format: "tabs",
      $ref: "basic_person.json",
      // properties: {
      //   title: "Person",
        // headerTemplate: "{{i}} - {{self.name}}",
        // oneOf: [
        //   {
        //     $ref: "basic_person.json",
        //     title: "Basic Person"
        //   },
        //   {
        //     $ref: "person.json",
        //     title: "Complex Person"
        //   }
        // ]
      // }
    },
    
    // Seed the form with a starting value
    //startval: starting_value,
    
    // Disable additional properties
    no_additional_properties: true,
    
    // Require all properties by default
    required_by_default: true
  });

})
