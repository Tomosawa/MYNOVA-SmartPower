<template>
  <v-row justify="center">
    <v-dialog v-model="dialog" persistent width="auto">
      <v-card>
        <v-card-title class="text-h5">
          保存完成
        </v-card-title>
        <v-card-text>WIFI配置保存成功，需要立即进行连接吗？</v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn color="green-darken-1" variant="text" @click="dialog = false">
            否
          </v-btn>
          <v-btn color="green-darken-1" variant="text" @click="connectWifi">
            是
          </v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>
  </v-row>

  <v-row justify="center">
    <v-dialog v-model="cleardialog" persistent width="auto">
      <v-card>
        <v-card-title class="text-h5">
          清除WI-FI设置？
        </v-card-title>
        <v-card-text>清除WIFI设置后将断开网络连接，确认清除？</v-card-text>
        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn color="green-darken-1" variant="text" @click="cleardialog = false">
            否
          </v-btn>
          <v-btn color="green-darken-1" variant="text" @click="deleteConfig">
            是
          </v-btn>
        </v-card-actions>
      </v-card>
    </v-dialog>
  </v-row>

  <v-card class="my-10" />

  <v-card class="mx-auto pa-12 pb-8" max-width="448">
    <v-form ref="form" v-model="form" @submit.prevent="onSubmit">
      <div class="text-subtitle-1 text-medium-emphasis">WIFI网络</div>

      <v-combobox v-model="selectedItem"
      :items="wifi_items"
      item-title="SSID"
      density="compact" placeholder="WI-FI网络名称"
      variant="outlined" :readonly="loading" :rules="[required]">
      <template v-slot:prepend-inner>
           <!-- <img src="@/assets/wifi.svg" width="16" height="16" style="margin-right: 5px;"/> -->
            <inline-svg :src="icon_wifi" width="16" height="16" style="margin-right: 5px;"></inline-svg>
      </template>
      </v-combobox>

      <div class="text-subtitle-1 text-medium-emphasis d-flex align-center justify-space-between">
        WIFI密码
      </div>

      <v-text-field v-model="password"
        :type="visible ? 'text' : 'password'" :readonly="loading" :rules="[required]" density="compact"
        placeholder="输入WIFI密码" variant="outlined">
        <template v-slot:prepend-inner>
              <!-- <img src="@/assets/password.svg" width="16" height="16" style="margin-right: 5px;"/> -->
              <inline-svg :src="icon_password" width="16" height="16" style="margin-right: 5px;"></inline-svg>
        </template>
        <template v-slot:append-inner>
              <!-- <img v-if="visible" src="@/assets/eye.svg" width="24" height="24" @click="visible = !visible"/>
              <img v-else src="@/assets/eye-close.svg" width="24" height="24" @click="visible = !visible"/> -->
              <inline-svg v-if="visible" :src="icon_eye" width="24" height="24" @click="visible = !visible"></inline-svg>
              <inline-svg v-else :src="icon_eyeclose" width="24" height="24" @click="visible = !visible"></inline-svg>
        </template>
        </v-text-field>

      <v-btn block variant="elevated" :disabled="!form" :loading="loading" color="success" size="large" type="submit">
        保存配置
      </v-btn>

      <v-btn color="error" class="mt-4" block @click="clearConfig" :disabled="!form" :loading="loading" size="large">
        删除配置
      </v-btn>
    </v-form>
  </v-card>

</template>
<script>
  import axios from 'axios';
  import InlineSvg from 'vue-inline-svg';
  import svg_wifi from '../assets/wifi.svg';
  import svg_password from '../assets/password.svg';
  import svg_eye from '../assets/eye.svg';
  import svg_eyeclose from '../assets/eye-close.svg';
  export default {
    components: {
       InlineSvg,
    },
    data: () => ({
      form: false,
      selectedItem: [],
      password: null,
      loading: false,
      visible: false,
      dialog: false,
      cleardialog: false,
      icon_wifi: svg_wifi,
      icon_password: svg_password,
      icon_eye: svg_eye,
      icon_eyeclose:svg_eyeclose,
      wifi_items :[
        {
          SSID: "Network1"
        },
      ],
    }),
    mounted() {
      console.log("WIFI config mounted");
      axios.get('/api/wifiscan').then(
        // 成功
        response => {
          console.log('请求成功了',response.data)
          this.wifi_items = response.data.WIFI_SCAN;
          this.selectedItem = response.data.WIFI_SSID;
          this.password = response.data.WIFI_PASSWORD;
        },
        // 失败
        error => {
          console.log('请求失败了',error.message)
        }
      );
    },
    methods: {
      onSubmit() {
        if (!this.form) return

        this.loading = true
        this.dialog = true

        var wifissid = "";
        if(typeof this.selectedItem === 'object')
          wifissid = this.selectedItem.SSID;
        else
          wifissid = this.selectedItem;

        var data =
        {
          SSID: wifissid,
          PWD: this.password
        };
        console.log(data);
        axios.post('/api/wifisave', data).then(
          response => {
            console.log(response.data);
            this.loading = false
          },
          error => {
            console.log(error);
            this.loading = false
          }
        );
        //setTimeout(() => (this.loading = false), 2000)
      },
      required(v) {
        return !!v || '不能为空'
      },
      connectWifi() {
        this.dialog = false
        this.loading = true
        axios.get('/api/wificonnect').then(
          // 成功
          response => {
            console.log('请求成功了',response.data)
            this.loading = false
          },
          // 失败
          error => {
            console.log('请求失败了',error.message)
            this.loading = false
          }
        );
      },
      clearConfig () {
        this.cleardialog = true;
      },
      deleteConfig() {
        axios.delete('/api/wificlear').then(
          // 成功
          response => {
            console.log('请求成功了',response.data)
            this.$refs.form.reset();
            this.cleardialog = false;
          },
          // 失败
          error => {
            console.log('请求失败了',error.message)
            this.cleardialog = false;
          }
        );
      },
    },
  }
</script>


<style>
</style>
