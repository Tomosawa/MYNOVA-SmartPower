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

  <v-card class="my-5" />

  <v-card class="mx-auto pa-12 pb-0" max-width="448">
    <v-form ref="form" v-model="form" @submit.prevent="onSubmit">
      <div class="text-subtitle-1 text-medium-emphasis">WIFI网络</div>

      <v-combobox v-model="ST_SSID" :items="wifi_items" item-title="SSID" density="compact" placeholder="WI-FI网络名称"
        variant="outlined" :readonly="loading" :rules="[required]" prepend-inner-icon="wifi">
      </v-combobox>

      <div class="text-subtitle-1 text-medium-emphasis d-flex align-center justify-space-between">
        WIFI密码
      </div>

      <v-text-field v-model="ST_PASSWORD" :type="WIFI_visible ? 'text' : 'password'" :readonly="loading" :rules="[required]"
        density="compact" placeholder="输入WIFI密码" variant="outlined" prepend-inner-icon="lock" :append-inner-icon="WIFI_visible ? 'visibility_off' : 'visibility'" @click:append-inner="WIFI_visible = !WIFI_visible">
      </v-text-field>

      <v-btn block variant="elevated" :disabled="!form" :loading="loading" color="success" size="large" type="submit">
        保存配置
      </v-btn>

      <v-btn color="error" class="mt-4" block @click="clearConfig" :disabled="!form" :loading="loading" size="large">
        删除配置
      </v-btn>
    </v-form>
    <v-switch color="success" v-model="ST_ENABLE" inset @change="onWifiEnableChanged" label="启用WIFI连接" style="height: 56px;"></v-switch>
  </v-card>


  <v-card class="mx-auto pa-12 pb-8" max-width="448">
    <v-form ref="AP_form" v-model="AP_form" @submit.prevent="onAPSubmit">
      <div class="text-subtitle-1 text-medium-emphasis">WIFI热点</div>
      <v-text-field v-model="AP_SSID" type="text" density="compact" placeholder="WIFI热点名称"
        variant="outlined" :readonly="loading" :rules="[required]" prepend-inner-icon="wifi_tethering">
      </v-text-field>

      <div class="text-subtitle-1 text-medium-emphasis d-flex align-center justify-space-between">
        热点密码
      </div>

      <v-text-field v-model="AP_PASSWORD" :type="AP_visible ? 'text' : 'password'" :readonly="loading" :rules="[required]"
        density="compact" placeholder="输入热点密码" variant="outlined" prepend-inner-icon="lock" :append-inner-icon="AP_visible ? 'visibility_off' : 'visibility'"  @click:append-inner="AP_visible = !AP_visible">
      </v-text-field>
      <v-btn block variant="elevated" :disabled="!AP_form" :loading="loading" color="success" size="large" type="submit">
        保存配置
      </v-btn>
      <v-switch color="success" v-model="AP_ENABLE" inset label="启用AP热点" style="height: 56px;" @change="onAPEnable"></v-switch>
    </v-form>
  </v-card>
  <v-dialog
      v-model="AP_dialog"
      max-width="400"
      persistent
    >
      <v-card
        prepend-icon="warning"
        text-color="red"
        text="警告：如果修改热点并忘记了密码，将导致再也无法连上设备进行设置，请谨慎操作！"
        title="确认修改热点?"
      >
      <v-chip color="red" class="ml-6 mr-6" variant="flat" rounded="0">
        请输入文字：我确认风险并修改
      </v-chip>
        <v-text-field v-model="AP_confirm" class="ml-6 mr-6 mt-2" :rules="[required]" density="compact" placeholder="请输入确认文字" variant="outlined"></v-text-field>
        <template v-slot:actions>
          <v-spacer></v-spacer>

          <v-btn rounded="0" variant="flat" @click="onAPConfirm" :disabled="!isConfirmValid" :color="isConfirmValid ? 'red-darken-1' : ''">
            确认修改
          </v-btn>

          <v-btn color="blue-darken-4" rounded="0" variant="flat" @click="AP_dialog = false">
            取消返回
          </v-btn>
        </template>
      </v-card>
    </v-dialog>
</template>
<script>
  import axios from 'axios';
  export default {
    components: {
    },
    data: () => ({
      form: false,
      AP_form: false,
      ST_SSID: '',
      ST_PASSWORD: '',
      ST_ENABLE: false,
      AP_SSID: '',
      AP_PASSWORD: '',
      AP_ENABLE: false,
      loading: false,
      WIFI_visible: false,
      AP_visible: false,
      dialog: false,
      cleardialog: false,
      AP_dialog: false,
      AP_confirm: '',
      wifi_items: [{
        SSID: "Network1"
      }, ],
    }),
    mounted() {
      console.log("WIFI config mounted");
      this.getWifiScan();
      this.getAPInfo();
    },
    methods: {
      onSubmit() {
        if (!this.form) return

        this.loading = true
        this.dialog = true

        var wifissid = "";
        if (typeof this.ST_SSID === 'object')
          wifissid = this.ST_SSID.SSID;
        else
          wifissid = this.ST_SSID;

        var data = {
          SSID: wifissid,
          PWD: this.ST_PASSWORD
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
      onAPSubmit(){
        if (!this.AP_form) return
        this.AP_confirm = '';
        this.AP_dialog = true;
      },
      required(v) {
        return !!v || '不能为空'
      },
      getWifiScan(){
        axios.get('/api/wifiscan').then(
          response => {
            console.log('请求成功了', response.data)
            this.wifi_items = response.data.WIFI_SCAN;
          },
          error => {
            console.log('请求失败了', error.message)
          }
        );
      },
      connectWifi() {
        this.dialog = false
        this.loading = true
        axios.get('/api/wificonnect').then(
          // 成功
          response => {
            console.log('请求成功了', response.data)
            this.loading = false
          },
          // 失败
          error => {
            console.log('请求失败了', error.message)
            this.loading = false
          }
        );
      },
      clearConfig() {
        this.cleardialog = true;
      },
      deleteConfig() {
        axios.delete('/api/wificlear').then(
          // 成功
          response => {
            console.log('请求成功了', response.data)
            this.$refs.form.reset();
            this.cleardialog = false;
          },
          // 失败
          error => {
            console.log('请求失败了', error.message)
            this.cleardialog = false;
          }
        );
      },
      onWifiEnableChanged(){
        //发送请求到/api/wifienable
        var data = {
          WIFI_ENABLE: this.ST_ENABLE
        };
        console.log(data);
        axios.post('/api/wifienable', data).then(
          response => {
            console.log(response.data);
          },
          error => {
            console.log('请求失败了', error.message)
          }
        );
      },
      //获取AP热点信息
      getAPInfo(){
        axios.get('/api/apwifiinfo').then(
          response => {
            console.log('请求成功了', response.data)
            this.AP_SSID = response.data.AP_SSID;
            this.AP_PASSWORD = response.data.AP_PWD;
            this.AP_ENABLE = response.data.AP_ENABLE;
            this.ST_SSID = response.data.WIFI_SSID;
            this.ST_PASSWORD = response.data.WIFI_PASSWORD;
            this.ST_ENABLE = response.data.WIFI_ENABLE;
          },
          error => {
            console.log('请求失败了', error.message)
          } 
        );
      },
      //保存AP热点信息
      saveAPInfo(){
        var data = {
          AP_SSID: this.AP_SSID,
          AP_PWD: this.AP_PASSWORD
        };
        console.log(data);
        axios.post('/api/apsave', data).then(
          response => {
            console.log(response.data);
          },
          error => {
            console.log('请求失败了', error.message)
          }
        );
      },
      //发送AP热点开关
      onAPEnable(){
        var data = {
          AP_ENABLE: this.AP_ENABLE
        };
        console.log(data);
        axios.post('/api/apenable', data).then(
          response => {
            console.log(response.data);
          },
          error => {
            console.log('请求失败了', error.message)
          }
        );
      },
      onAPConfirm(){
        this.AP_dialog = false;
        this.saveAPInfo();
      }
    },
    computed: {
      isConfirmValid() {
        return this.AP_confirm === '我确认风险并修改'
      }
    }
  }
</script>


<style>
</style>
