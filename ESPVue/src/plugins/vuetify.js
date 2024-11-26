/**
 * plugins/vuetify.js
 *
 * Framework documentation: https://vuetifyjs.com`
 */

// Styles
//import '@mdi/font/css/materialdesignicons.css'
import 'vuetify/styles'

// Composables
import { createVuetify } from 'vuetify'
import 'material-design-icons-iconfont/dist/material-design-icons.css'
import { aliases, md } from 'vuetify/iconsets/md'

// Components
import { VTimePicker } from 'vuetify/labs/VTimePicker'
import { VNumberInput } from 'vuetify/labs/VNumberInput'

// https://vuetifyjs.com/en/introduction/why-vuetify/#feature-guides
export default createVuetify({
  icons: {
      defaultSet: 'md',
      aliases,
      sets: {
        md,
      },
    },
 components: {
     VTimePicker,
     VNumberInput,
   },

})
