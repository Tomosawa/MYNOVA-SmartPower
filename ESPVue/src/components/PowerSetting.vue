<template>
  <v-card class="ma-2">
    <v-toolbar
      flat
      dense
    >
      <v-toolbar-title>
        <span class="text-subheading">电源状态</span>
      </v-toolbar-title>
    </v-toolbar>
  <v-card
    border
    density="compact"

    :title="powerstate"
    variant="text"
  >
    <template v-slot:prepend>
      <!-- <img src="@/assets/power.svg" width="24" height="24" style="margin-right: 5px;"/> -->
      <inline-svg :src="power" width="24" height="24" style="margin-right: 5px;"></inline-svg>
    </template>
    <template v-slot:append>
    <v-switch
      color="success"
      v-model="powersw"
      inset
      label="开关"
      style="height: 56px;"
      @change="switchChanged"
    ></v-switch>
    </template>
  </v-card>
</v-card>
   <v-card
      class="ma-2"
    >
      <v-toolbar
        flat
        dense
      >
        <v-toolbar-title>
          <span class="text-subheading">风扇转速</span>
        </v-toolbar-title>


      </v-toolbar>

      <v-card-text>
        <v-row
          class="mb-1"
          justify="space-between"
        >
          <v-col class="text-left">
            <!-- <img src="@/assets/fan.svg" width="24" height="24" style="margin-right: 10px; "/> -->
            <span
              class="text-h5 font-weight-light">
              <inline-svg :src="fan" width="24" height="24" style="margin-right: 10px;transform: translateY(3px);"></inline-svg>
            </span>
            <span
              class="text-h5 font-weight-light"
            >{{fanRPM}} RPM</span>

          </v-col>
          <v-col class="text-right"><span class="text-h6 font-weight-light me-1 ml-5">{{fanSpeed}}%</span>
            <v-fade-transition>
              <v-avatar
                :color="color"
                :style="{
                  animationDuration: animationDuration
                }"
                class="mb-1 v-avatar--metronome"
                size="12"
              ></v-avatar>
            </v-fade-transition></v-col>

        </v-row>

        <v-slider
          v-model="fanSpeed"
          :color="color"
          track-color="grey"
          min="0"
          max="100"
          :step="1"
          @end="onSpeedChanged"
        >
          <!-- <template v-slot:prepend>
            <v-btn
              size="small"
              variant="text"
              icon="mdi-minus"
              :color="color"
              @click="decrement"
            ></v-btn>
          </template>

          <template v-slot:append>
            <v-btn
              size="small"
              variant="text"
              icon="mdi-plus"
              :color="color"
              @click="increment"
            ></v-btn>
          </template> -->
        </v-slider>
      </v-card-text>
    </v-card>
</template>

<script>
import axios from 'axios';
import InlineSvg from 'vue-inline-svg';
import icon_power from '../assets/power.svg';
import icon_fan from '../assets/fan.svg';
export default {
    components: {
       InlineSvg,
    },
    data: () => ({
      fanSpeed: 0,
      fanRPM: 0,
      powersw: false,
      powerstate: '未启动',
      intervalId: 0,
      firstFetch: true,
      power: icon_power,
      fan: icon_fan,
    }),
    mounted() {
      this.fetchData();
      this.intervalId = setInterval(this.fetchData, 1000); // 每 1 秒发送一次请求
    },
    computed: {
      color () {
        if (this.fanSpeed < 25) return 'indigo'
        if (this.fanSpeed < 40) return 'teal'
        if (this.fanSpeed < 75) return 'green'
        if (this.fanSpeed >= 75) return 'orange'
        return 'red'
      },
      animationDuration () {
        return `${60 / this.fanSpeed}s`
      },
    },

    methods: {
      async fetchData(){
        axios.get('/api/psusetting').then(
          response => {
            console.log('请求成功了',response.data)
            //第一次请求则更新开关和速度值
            if(this.firstFetch)
            {
              this.powersw = response.data.POWER_SW;
              this.fanSpeed = response.data.FAN_SETTING;
              this.firstFetch = false;
            }
            this.fanRPM = response.data.READ_FAN_SPEED_1;
            if(response.data.POWER_SW)
            {
              var time  = response.data.RUN_TIME;
              // 获取天数
              var days = Math.floor(time / (1000 * 60 * 60 * 24));
              // 获取小时、分钟、秒
              var hours = Math.floor((time % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
              var minutes = Math.floor((time % (1000 * 60 * 60)) / (1000 * 60));
              var seconds = Math.floor((time % (1000 * 60)) / 1000);
              // 格式化输出
              var formatted = `${days}天${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
              this.powerstate = formatted;
            }
            else
            {
              this.powerstate = '未启动';
            }
          },
          error => {
            console.log('请求失败了',error.message)
          }
        );
      },
      decrement () {
        this.fanSpeed--
      },
      increment () {
        this.fanSpeed++
      },
      switchChanged() {
        console.log('Changed ' + this.powersw );
        var data =
        {
          "POWER_ON" : this.powersw,
        };
        console.log(data);
        axios.put('/api/psupower', data).then(
          response => {
            console.log(response.data);
          },
          error => {
            console.log(error);
          }
        );
      },
      onSpeedChanged() {
        console.log('Changed ' + this.fanSpeed );
        var data =
        {
          "FAN_SPEED" : this.fanSpeed,
        };
        console.log(data);
        axios.put('/api/psufan', data).then(
          response => {
            console.log(response.data);
          },
          error => {
            console.log(error);
          }
        );
      },
    },
    beforeUnmount() {
      clearInterval(this.intervalId); // 组件卸载前清除定时器
    }
  }

</script>

<style>
  @keyframes metronome-example {
    from {
      transform: scale(.5);
    }

    to {
      transform: scale(1);
    }
  }

  .v-avatar--metronome {
    animation-name: metronome-example;
    animation-iteration-count: infinite;
    animation-direction: alternate;
  }
</style>
