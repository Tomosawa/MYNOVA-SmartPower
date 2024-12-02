<template>
  <v-card class="ma-2">
    <v-toolbar flat dense>
      <v-toolbar-title>
        <span class="text-subheading">电源状态</span>
      </v-toolbar-title>
    </v-toolbar>
    <v-card border density="compact" :title="powerstate" variant="text">
      <template v-slot:prepend>
        <v-icon icon="power_settings_new"></v-icon>
      </template>
      <template v-slot:append>
        <v-switch color="success" v-model="powersw" inset label="开关" style="height: 56px;"
          @change="switchChanged"></v-switch>
      </template>
    </v-card>
  </v-card>
  <v-card class="ma-2">
    <v-toolbar flat dense>
      <v-toolbar-title>
        <span class="text-subheading">风扇转速</span>
      </v-toolbar-title>
    </v-toolbar>
    <v-card-text>
      <v-row class="mb-1" justify="space-between">
        <v-col class="text-left">
          <span class="text-h5 font-weight-light">
            <inline-svg :src="fan_svg" width="24" height="24"
              style="margin-right: 10px;transform: translateY(3px);"></inline-svg>
          </span>
          <span class="text-h5 font-weight-light">{{ fanRPM }} RPM</span>

        </v-col>
        <v-col class="text-right"><span class="text-h6 font-weight-light me-1 ml-5">{{ fanSpeed }}%</span>
          <v-fade-transition>
            <v-avatar :color="color" :style="{ animationDuration: animationDuration }" class="mb-1 v-avatar--metronome"
              size="12"></v-avatar>
          </v-fade-transition></v-col>
      </v-row>
      <v-slider v-model="fanSpeed" :color="color" track-color="grey" min="0" max="100" :step="1" @end="onSpeedChanged">
      </v-slider>
    </v-card-text>
  </v-card>
  <v-card class="ma-2">
    <v-toolbar flat dense>
      <v-toolbar-title>
        <span class="text-subheading">上电开机</span>
      </v-toolbar-title>
    </v-toolbar>
    <v-card border density="compact" title="上电自动开机" variant="text">
      <template v-slot:prepend>
        <v-icon icon="power"></v-icon>
      </template>
      <template v-slot:append>
        <v-switch color="success" v-model="acPowerOn" inset :label="acPowerLabel" style="height: 56px;"
          @change="onACPowerOnChanged"></v-switch>
      </template>
    </v-card>
  </v-card>
  <v-card class="ma-2">
    <v-toolbar flat dense>
      <v-toolbar-title>
        <span class="text-subheading">倒计时关机</span>
      </v-toolbar-title>
    </v-toolbar>
    <v-card border density="compact" title="倒计时关机" variant="text">
      <template v-slot:prepend>
        <v-icon icon="history"></v-icon>
      </template>
      <template v-slot:append>
        <v-switch color="success" v-model="countDownPowerOff" inset :label="countDownPowerOffLabel"
          style="height: 56px;" @change="onCountDownPowerOffChanged" :disabled="!powersw"></v-switch>
      </template>
      <v-container>
        <v-row>
          <v-col>
            <v-text-field v-show="!countDownPowerOff" v-model="countDownTime" :active="countDownTimePicker"
              :focus="countDownTimePicker" :disabled="!powersw" readonly>
              <!-- <v-menu v-model="countDownTimePicker" :close-on-content-click="false" activator="parent"
                transition="scale-transition">
                <v-time-picker v-if="countDownTimePicker" v-model="countDownTime" color="green-lighten-1" format="24hr"
                  full-width></v-time-picker>
              </v-menu> -->
              <v-dialog v-model="countDownTimePicker" activator="parent" width="auto">
                <v-card>
                  <v-time-picker v-if="countDownTimePicker" v-model="countDownTime" color="green-lighten-1"
                    format="24hr" full-width>
                  </v-time-picker>
                  <v-card-actions>
                    <v-spacer></v-spacer>
                    <v-btn color="primary" text="确定" variant="tonal" @click="countDownTimePicker = false" :disabled="!isCountDownTimeChanged"></v-btn>
                  </v-card-actions>
                </v-card>
              </v-dialog>
            </v-text-field>

            <div v-show="countDownPowerOff" class="v-time-picker-controls__time">
              <button type="button"
                class="v-btn v-theme--dark v-btn--density-default v-btn--size-default v-btn--variant-tonal v-time-picker-controls__time__btn v-time-picker-controls__time--with-ampm__btn">
                <span class="v-btn__overlay"></span>
                <span class="v-btn__underlay"></span>
                <span class="v-btn__content" data-no-activator="">{{ formattedHour }}</span>
              </button>
              <span class="v-time-picker-controls__time__separator">:</span>
              <button type="button"
                class="v-btn v-theme--dark v-btn--density-default v-btn--size-default v-btn--variant-tonal v-time-picker-controls__time__btn v-time-picker-controls__time--with-ampm__btn">
                <span class="v-btn__overlay"></span>
                <span class="v-btn__underlay"></span>
                <span class="v-btn__content" data-no-activator="">{{ formattedMinute }}</span>
              </button>
              <span class="v-time-picker-controls__time__separator">:</span>
              <button type="button"
                class="v-btn v-theme--dark v-btn--density-default v-btn--size-default v-btn--variant-tonal v-time-picker-controls__time__btn v-time-picker-controls__time--with-ampm__btn">
                <span class="v-btn__overlay"></span>
                <span class="v-btn__underlay"></span>
                <span class="v-btn__content" data-no-activator="">{{ formattedSecond }}</span>
              </button>
            </div>
          </v-col>
        </v-row>
      </v-container>
    </v-card>
  </v-card>
  <v-card class="ma-2" border density="compact">
    <v-toolbar flat dense>
      <v-toolbar-title>
        <div class="d-flex align-center justify-space-between mr-5">
          <span class="text-subheading">定时开关机</span>
          <v-btn icon="add" size="small" variant="tonal" @click="setTimeDialog = true"></v-btn>
        </div>
      </v-toolbar-title>
    </v-toolbar>
    <v-card title="" variant="text">
      <template v-slot:prepend>
        <v-icon icon="schedule"></v-icon>
        <span class="text-subheading ml-2" style="font-size: 1.25rem;">{{ formattedTime }}</span>
      </template>
      <template v-slot:append>
        <v-btn 
          size="small" 
          icon="sync" 
          class="ml-2" 
          :class="{ 'rotate-animation': isUpdating }"
          :disabled="isUpdating"
          @click="onUpdateTime"
        ></v-btn>
      </template>
    </v-card>
    <v-divider></v-divider>
    <v-card v-for="taskItem in setTimeTask" :key="taskItem.TaskId" :title="getTaskTitle(taskItem)"
      :subtitle="getTaskSubtitle(taskItem)">
      <template v-slot:append>
        <v-switch color="success" v-model="taskItem.ModeEnable" inset style="height: 56px;"
          @change="onSetTimeTaskChanged(taskItem)"></v-switch>
        <v-btn size="small" icon="delete" class="ml-2" @click="onDelSetTime(taskItem.TaskId)"></v-btn>
      </template>
    </v-card>
  </v-card>

  <v-dialog v-model="setTimeDialog" max-width="600">
    <v-form ref="form" v-model="setTimeFormValid" @submit.prevent="onAddSetTime">
      <v-card title="添加定时任务">
        <v-card-text>
          <v-row dense>
            <v-col cols="12">
              <v-select v-model="setTimeType" label="类型" :items="setTimeTypeItems" placeholder="类型" item-title="label"
                item-value="value" :return-object="false" variant="solo-filled"
                @update:modelValue="onSetTimeTypeChanged" required></v-select>
            </v-col>
          </v-row>
          <v-row dense>
            <v-col cols="12">
              <v-select v-model="setTime_Repeat" label="重复" :items="setTime_Repeat_Items" placeholder="重复"
                item-title="label" item-value="value" :return-object="false" variant="solo-filled"
                @update:modelValue="onSetTimeRepeatChanged" ref="setTime_Repeat_Combo" :rules="[weekRequired]"
                required></v-select>
            </v-col>
          </v-row>
          <v-row dense v-show="setTimeShowWeek" no-gutters>
            <v-col cols="auto" v-for="item in setTimeRepeatWeekItems" :key="item.value">
              <v-checkbox v-model="setTimeRepeatWeek" :label="item.label" :value="item.value"
                @change="() => this.$refs.setTime_Repeat_Combo.validate()"></v-checkbox>
            </v-col>
          </v-row>
          <v-row dense v-if="setTimePowerOnShow">
            <v-col cols="12">
              <v-text-field v-model="setTimePowerOn" :active="setTimePowerOnPicker" :focus="setTimePowerOnPicker"
                label="开机时间" readonly :rules="[required]" required>
                <v-dialog v-model="setTimePowerOnPicker" activator="parent" width="auto">
                  <v-card>
                    <v-time-picker v-if="setTimePowerOnPicker" v-model="setTimePowerOn" color="green-lighten-1"
                      format="24hr" full-width>
                    </v-time-picker>
                    <v-card-actions>
                      <v-spacer></v-spacer>
                      <v-btn color="primary" text="确定" variant="tonal" @click="setTimePowerOnPicker = false" :disabled="!isPowerOnTimeChanged"></v-btn>
                    </v-card-actions>
                  </v-card>
                </v-dialog>
              </v-text-field>
            </v-col>
          </v-row>
          <v-row dense v-if="setTimePowerOffShow">
            <v-col cols="12">
              <v-text-field v-model="setTimePowerOff" :active="setTimePowerOffPicker" :focus="setTimePowerOffPicker"
                label="关机时间" readonly :rules="[required]" required>
                <v-dialog v-model="setTimePowerOffPicker" activator="parent" width="auto">
                  <v-card>
                    <v-time-picker v-if="setTimePowerOffPicker" v-model="setTimePowerOff" color="green-lighten-1"
                      format="24hr" full-width>
                    </v-time-picker>
                    <v-card-actions>
                      <v-spacer></v-spacer>
                      <v-btn color="primary" text="确定" variant="tonal" @click="setTimePowerOffPicker = false" :disabled="!isPowerOffTimeChanged"></v-btn>
                    </v-card-actions>
                  </v-card>
                </v-dialog>
              </v-text-field>
            </v-col>
          </v-row>
        </v-card-text>

        <v-divider></v-divider>

        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn text="取消" variant="plain" @click="setTimeDialog = false"></v-btn>
          <v-btn color="primary" text="确定" variant="tonal" type="submit" :disabled="!setTimeFormValid"></v-btn>
        </v-card-actions>
      </v-card>
    </v-form>
  </v-dialog>

  <v-dialog v-model="delSetTimeDialog" persistent width="auto">
    <v-card>
      <v-card-title class="text-h5">
        确认删除
      </v-card-title>
      <v-card-text>您确定要删除当前任务吗？</v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn color="green-darken-1" variant="text" @click="delSetTimeDialog = false">
          取消
        </v-btn>
        <v-btn color="red-lighten-1" variant="flat" @click="deleteSetTimeTask">
          删除
        </v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>

  <v-card class="ma-2" border density="compact">
    <v-toolbar flat dense>
      <v-toolbar-title>
        <div class="d-flex align-center justify-space-between mr-5">
          <span class="text-subheading">条件关机</span>
          <v-btn icon="add" size="small" variant="tonal" @click="conditionTaskDialog = true"></v-btn>
        </div>
      </v-toolbar-title>
    </v-toolbar>
    <v-card v-for="taskItem in conditionTask" :key="taskItem.TaskId" :title="getConditionTaskTitle(taskItem)">
      <template v-slot:append>
        <v-switch color="success" v-model="taskItem.ModeEnable" inset style="height: 56px;"
          @change="onConditionTaskChanged(taskItem)"></v-switch>
        <v-btn size="small" icon="delete" class="ml-2" @click="onDelConditionTask(taskItem.TaskId)"></v-btn>
      </template>
    </v-card>
  </v-card>

  <v-dialog v-model="conditionTaskDialog" max-width="600">
    <v-form ref="form" v-model="conditionFormValid" @submit.prevent="onAddConditionTask">
      <v-card title="添加关机条件">
        <v-card-text>
          <v-row dense>
            <v-col col="12">
              <v-select v-model="conditionType" :items="conditionTypeItems" item-title="label" item-value="value"
                :return-object="false" variant="solo-filled" required></v-select>
              <v-select v-model="conditionQuantity" :items="conditionQuantityItems" item-title="label"
                item-value="value" :return-object="false" variant="solo-filled" required></v-select>
              <v-select v-model="compareType" :items="compareTypeItems" item-title="label" item-value="value"
                :return-object="false" variant="solo-filled" required></v-select>
              <v-number-input control-variant="split" :min="0" v-model="conditionValue" variant="solo-filled"
                 required
                :suffix="conditionQuantity === 0 ? 'A' : 'W'"></v-number-input>
              <v-number-input control-variant="split" :min="1" v-model="conditionMinutes" variant="solo-filled"
                type="number" required prefix=" 持续" suffix="分钟后关机 "></v-number-input>
            </v-col>
          </v-row>
        </v-card-text>

        <v-divider></v-divider>

        <v-card-actions>
          <v-spacer></v-spacer>
          <v-btn text="取消" variant="plain" @click="conditionTaskDialog = false"></v-btn>
          <v-btn color="primary" text="确定" variant="tonal" type="submit" :disabled="!conditionFormValid"></v-btn>
        </v-card-actions>
      </v-card>
    </v-form>
  </v-dialog>

  <v-dialog v-model="delConditionDialog" persistent width="auto">
    <v-card>
      <v-card-title class="text-h5">
        确认删除
      </v-card-title>
      <v-card-text>您确定要删除当前条件吗？</v-card-text>
      <v-card-actions>
        <v-spacer></v-spacer>
        <v-btn color="green-darken-1" variant="text" @click="delConditionDialog = false">
          取消
        </v-btn>
        <v-btn color="red-lighten-1" variant="flat" @click="deleteConditionTask">
          删除
        </v-btn>
      </v-card-actions>
    </v-card>
  </v-dialog>
</template>

<script>
import axios from 'axios';
import InlineSvg from 'vue-inline-svg';
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
    acPowerOn: false,
    countDownPowerOff: false,
    intervalId: 0,
    fan_svg: icon_fan,
    countDownTime: '01:00',
    countDownTimePicker: false,
    countDown_Hour: 1,
    countDown_Minute: 0,
    countDown_Second: 0,
    countDownInterval: null,
    systemTime: new Date(),
    setTimeDialog: false,
    setTime_Repeat: 0,
    setTime_Repeat_Items: [{ label: '执行一次', value: 0 }, { label: '每天', value: 1 }, { label: '自定义', value: 2 }],
    setTimePowerOn: '',
    setTimePowerOff: '',
    setTimePowerOffPicker: false,
    setTimePowerOnPicker: false,
    setTimeShowWeek: false,
    setTimeType: 0,
    setTimeTypeItems: [{ label: '时间段定时', value: 0 }, { label: '开机定时', value: 1 }, { label: '关机定时', value: 2 }],
    setTimePowerOnShow: true,
    setTimePowerOffShow: true,
    setTimeRepeatWeek: [0, 1, 2, 3, 4, 5, 6],
    setTimeRepeatWeekItems: [
      { label: '星期一', value: 1 },
      { label: '星期二', value: 2 },
      { label: '星期三', value: 3 },
      { label: '星期四', value: 4 },
      { label: '星期五', value: 5 },
      { label: '星期六', value: 6 },
      { label: '星期日', value: 0 },
    ],
    setTimeFormValid: false,
    setTimeTask: [
      { TaskId: 0, TimeType: 0, RepeatMode: 0, RepeatWeek: [1, 2, 3, 4, 5, 6, 0], PowerOnTime: '9:00', PowerOffTime: '12:00', ModeEnable: true }
    ],
    delSetTimeDialog: false,
    delSetTimeTaskId: 0,
    delConditionDialog: false,
    delConditionTaskId: 0,
    conditionTaskDialog: false,
    conditionTask: [
      { TaskId: 0, ConditionType: 0, ConditionQuantity: 0, ConditionValue: 0, CompareType: 0, ConditionMinutes: 0, ModeEnable: true }
    ],
    conditionFormValid: false,
    conditionType: 1,
    conditionTypeItems: [
      { label: '输入', value: 0 },
      { label: '输出', value: 1 }
    ],
    conditionQuantity: 0,
    conditionQuantityItems: [
      { label: '电流', value: 0 },
      { label: '功率', value: 1 }
    ],
    conditionValue: 0.5,
    compareType: 1,
    compareTypeItems: [
      { label: '>', value: 0 },
      { label: '<', value: 1 }
    ],
    conditionMinutes: 5,
    isUpdating: false,
    originalPowerOnTime: '',
    originalPowerOffTime: '',
    originalCountDownTime: '',
  }),
  mounted() {
    this.fetchData();
    this.intervalId = setInterval(this.fetchData, 1000); // 每 1 秒发送一次请求
    this.fetchTasks(); // 添加这行来获取任务数据
  },
  computed: {
    color() {
      if (this.fanSpeed < 25) return 'indigo'
      if (this.fanSpeed < 40) return 'teal'
      if (this.fanSpeed < 75) return 'green'
      if (this.fanSpeed >= 75) return 'orange'
      return 'red'
    },
    animationDuration() {
      return `${60 / this.fanSpeed}s`
    },
    acPowerLabel() {
      return this.acPowerOn ? '启用' : '禁用';
    },
    countDownPowerOffLabel() {
      return this.countDownPowerOff ? '启用' : '禁用';
    },
    formattedHour() {
      return this.countDown_Hour.toString().padStart(2, '0');
    },
    formattedMinute() {
      return this.countDown_Minute.toString().padStart(2, '0');
    },
    formattedSecond() {
      return this.countDown_Second.toString().padStart(2, '0');
    },
    formattedTime() {
      return this.systemTime.toLocaleString('zh-CN', {
        year: 'numeric',
        month: '2-digit',
        day: '2-digit',
        hour: '2-digit',
        minute: '2-digit',
        second: '2-digit',
        hour12: false,
      }).replace(/\//g, '/');
    },
    isPowerOnTimeChanged() {
      return this.setTimePowerOn !== this.originalPowerOnTime;
    },
    isPowerOffTimeChanged() {
      return this.setTimePowerOff !== this.originalPowerOffTime;
    },
    isCountDownTimeChanged() {
      return this.countDownTime !== this.originalCountDownTime;
    },
  },
  watch: {
    setTimePowerOnPicker(newValue) {
      // 打开选择器时记录初始值
      if (newValue) {
        this.originalPowerOnTime = this.setTimePowerOn;
      }
    },
    setTimePowerOffPicker(newValue) {
      if (newValue) {
        this.originalPowerOffTime = this.setTimePowerOff;
      }
    },
    countDownTimePicker(newValue) {
      if (newValue) {
        this.originalCountDownTime = this.countDownTime;
      }
    }
  },
  methods: {
    //定时每秒钟获取数据
    async fetchData() {
      axios.get('/api/psusetting').then(
        response => {
          console.log('请求成功了', response.data)
          this.powersw = response.data.POWER_SW;
          this.fanSpeed = response.data.FAN_SETTING;
          this.acPowerOn = response.data.AC_POWER_ON;
          this.fanRPM = response.data.READ_FAN_SPEED_1;
          if (response.data.POWER_SW) {
            var time = response.data.RUN_TIME;
            // 获取天数
            var days = Math.floor(time / (1000 * 60 * 60 * 24));
            // 获取小时、分钟、秒
            var hours = Math.floor((time % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
            var minutes = Math.floor((time % (1000 * 60 * 60)) / (1000 * 60));
            var seconds = Math.floor((time % (1000 * 60)) / 1000);
            // 格式化输出
            var formatted =
              `${days}天${hours.toString().padStart(2, '0')}:${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}`;
            this.powerstate = formatted;
          } else {
            this.powerstate = '未启动';
          }
        },
        error => {
          console.log('请求失败了', error.message)
        }
      );
      this.onTimeTick();
    },
    //获取所有的任务
    async fetchTasks() {
      try {
        const response = await axios.get('/api/tasks/all');
        console.log('获取任务数据成功:', response.data);

        // 处理倒计时关机状态
        if (response.data.shutdownTimer) {
          this.countDownPowerOff = response.data.shutdownTimer.active;
          if (this.countDownPowerOff) {
            const totalSeconds = response.data.shutdownTimer.remainingSeconds;
            this.countDown_Hour = Math.floor(totalSeconds / 3600);
            this.countDown_Minute = Math.floor((totalSeconds % 3600) / 60);
            this.countDown_Second = totalSeconds % 60;

            // 如果倒计时没有处于激活状态，启动倒计时
            if (!this.countDownInterval) {
              this.startCountDown();
            }
          } else {
            this.stopCountDown();
          }
        }

        // 处理时间段任务
        if (response.data.periodTasks) {
          this.setTimeTask = response.data.periodTasks.map(task => ({
            TaskId: task.id,
            TimeType: task.type, // 使用服务器返回的任务类型
            RepeatMode: task.repeatType,
            RepeatWeek: task.weekDays,
            PowerOnTime: task.type !== 2 ? `${String(task.startHour).padStart(2, '0')}:${String(task.startMinute).padStart(2, '0')}` : '',
            PowerOffTime: task.type !== 1 ? `${String(task.endHour).padStart(2, '0')}:${String(task.endMinute).padStart(2, '0')}` : '',
            ModeEnable: task.enabled
          }));
        }

        // 处理条件任务
        if (response.data.conditionTasks) {
          this.conditionTask = response.data.conditionTasks.map(task => ({
            TaskId: task.id,
            ConditionType: task.conditionType,
            ConditionQuantity: task.conditionQuantity,
            ConditionValue: task.conditionValue,
            CompareType: task.compareType,
            ConditionMinutes: task.conditionMinutes,
            ModeEnable: task.enabled
          }));
        }

        // 获取时间
        if (response.data.currentTime) {
          this.systemTime = new Date(response.data.currentTime);
        }
      } catch (error) {
        console.error('获取任务数据失败:', error);
      }
    },
    //页面操作函数===============================================
    decrement() {
      this.fanSpeed--
    },
    increment() {
      this.fanSpeed++
    },
    
    weekRequired(v) {
      if (this.setTime_Repeat == 2 &&
        this.setTimeRepeatWeek.length <= 0) {
        return '必须至少勾选一个！';
      }
      return true;
    },
    required(v) {
      return !!v || '不能为空'
    },
    //开关机操作=================================================
    switchChanged() {
      console.log('Changed ' + this.powersw);
      var data = {
        "POWER_ON": this.powersw,
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
    //风扇速度操作===============================================
    onSpeedChanged() {
      console.log('Changed ' + this.fanSpeed);
      var data = {
        "FAN_SPEED": this.fanSpeed,
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
    //上电自动开机===============================================
    async onACPowerOnChanged() {
      //发送请求到/api/acpoweron
      const response = await axios.post('/api/acpoweron', {
        "AC_POWER_ON": this.acPowerOn
      });
      console.log('上电自动开机:', response.data);
    },
    //同步时间=================================================
    async onUpdateTime() {
      this.isUpdating = true;
      const response = await axios.post('/api/synctime', {});
      console.log('发送同步时间命令:', response.data);
      
      // 等待5秒后调用fetchTasks并恢复按钮状态
      setTimeout(() => {
        this.fetchTasks();
        this.isUpdating = false;
      }, 5000);
    },
    onTimeTick() {
      // 时间加1秒
      this.systemTime = new Date(this.systemTime.getTime() + 1000);
    },

    async onCountDownPowerOffChanged() {
      try {
        if (this.countDownPowerOff) {
          const parts = this.countDownTime.match(/(\d{2}):(\d{2})(?::(\d{2}))?/);
          if (parts) {
            this.countDown_Hour = parseInt(parts[1], 10);
            this.countDown_Minute = parseInt(parts[2], 10);
            // 如果存在秒，则也解析秒，否则默认为0
            this.countDown_Second = parts[3] ? parseInt(parts[3], 10) : 0;

            const totalSeconds =
              this.countDown_Hour * 3600 +
              this.countDown_Minute * 60 +
              this.countDown_Second;

            await axios.post('/api/shutdown/timer', {
              seconds: totalSeconds
            });
            this.startCountDown();
          }
        } else {
          this.stopCountDown();
          await axios.delete('/api/shutdown/timer');
        }
      } catch (error) {
        console.error('更新倒计时状态失败:', error);
        // 恢复之前的状态
        this.countDownPowerOff = !this.countDownPowerOff;
      }
    },
    //设置时间段任务=============================================
    onSetTimeRepeatChanged() {
      console.log(this.setTime_Repeat);
      if (this.setTime_Repeat == 2) {
        this.setTimeShowWeek = true;
      }
      else {
        this.setTimeShowWeek = false;
      }
    },
    onSetTimeTypeChanged() {
      if (this.setTimeType == 0) {
        this.setTimePowerOnShow = true;
        this.setTimePowerOffShow = true;
      }
      else if (this.setTimeType == 1) {
        this.setTimePowerOnShow = true;
        this.setTimePowerOffShow = false;
      }
      else if (this.setTimeType == 2) {
        this.setTimePowerOnShow = false;
        this.setTimePowerOffShow = true;
      }
    },
    async onAddSetTime() {
      try {
        const data = {
          type: this.setTimeType,
          startHour: this.setTimePowerOn ? parseInt(this.setTimePowerOn.split(':')[0]) : 0,
          startMinute: this.setTimePowerOn ? parseInt(this.setTimePowerOn.split(':')[1]) : 0,
          endHour: this.setTimePowerOff ? parseInt(this.setTimePowerOff.split(':')[0]) : 0,
          endMinute: this.setTimePowerOff ? parseInt(this.setTimePowerOff.split(':')[1]) : 0,
          repeatType: this.setTime_Repeat,
          weekDays: this.setTimeRepeatWeek,
          enabled: true
        };

        await axios.post('/api/tasks/period/add', data);
        this.setTimeDialog = false;
        await this.fetchTasks(); // 重新获取所有任务
      } catch (error) {
        console.error('添加定时任务失败:', error);
      }
    },
    async onSetTimeTaskChanged(taskItem) {
      console.log(taskItem);
      console.log(taskItem.ModeEnable);

      // 发送请求到/api/tasks/period/toggle
      const response = await axios.post('/api/tasks/period/toggle', {
        taskId: taskItem.TaskId,
        toggle: taskItem.ModeEnable
      });
      console.log('切换时间段任务状态:', response.data);
    },
    onDelSetTime(taskId) {
      this.delSetTimeTaskId = taskId;
      this.delSetTimeDialog = true;
    },
    async deleteSetTimeTask() {
      this.setTimeTask = this.setTimeTask.filter(task => task.TaskId !== this.delSetTimeTaskId);
      this.delSetTimeDialog = false;
      // 发送请求到/api/tasks/period/delete
      const response = await axios.post('/api/tasks/period/delete', {
        taskId: this.delSetTimeTaskId
      });
      console.log('删除时间段任务:', response.data);
    },
    async onAddConditionTask() {
      this.conditionTaskDialog = false;
      var data = {
        TaskId: this.conditionTask.length,
        ConditionType: this.conditionType,
        ConditionQuantity: this.conditionQuantity,
        ConditionValue: this.conditionValue,
        CompareType: this.compareType,
        ConditionMinutes: this.conditionMinutes,
        ModeEnable: true,
      }
      console.log(data);

      //发送请求到/api/tasks/condition添加任务
      const response = await axios.post('/api/tasks/condition/add', data);
      console.log('添加条件任务:', response.data);
      //重新获取任务数据
      await this.fetchTasks();  
    },
    async onConditionTaskChanged(taskItem) {
      console.log(taskItem);
      console.log(taskItem.ModeEnable);

      //发送请求到/api/tasks/condition/toggle
      const response = await axios.post('/api/tasks/condition/toggle', {
        taskId: taskItem.TaskId,
        toggle: taskItem.ModeEnable
      });
      console.log('切换条件任务状态:', response.data);
    },
    onDelConditionTask(taskId) {
      this.delConditionTaskId = taskId;
      this.delConditionDialog = true;
    },
    async deleteConditionTask() {
      this.conditionTask = this.conditionTask.filter(task => task.TaskId !== this.delConditionTaskId);
      this.delConditionDialog = false;

      // 发送请求到/api/tasks/condition/delete
      const response = await axios.post('/api/tasks/condition/delete', {
        taskId: this.delConditionTaskId
      });
      console.log('删除条件任务:', response.data);
    },
    stopCountDown() {
      clearInterval(this.countDownInterval);
    },
    startCountDown() {
      if (this.countDownInterval) {
        clearInterval(this.countDownInterval);
      }
      this.countDownInterval = setInterval(() => {
        if (this.countDown_Second > 0) {
          this.countDown_Second--;
        } else {
          if (this.countDown_Minute > 0) {
            this.countDown_Minute--;
            this.countDown_Second = 59;
          } else {
            if (this.countDown_Hour > 0) {
              this.countDown_Hour--;
              this.countDown_Minute = 59;
              this.countDown_Second = 59;
            } else {
              clearInterval(this.countDownInterval);
              // 这里可以添加倒计时结束的逻辑
              this.countDownPowerOff = false;
              console.log('Countdown finished!');
            }
          }
        }
      }, 1000);
    },

    getWeekDaysString(weekDays) {
      const weekMap = ['日', '一', '二', '三', '四', '五', '六'];

      // 如果传入的是对象，提取对象的键作为数组
      const weekDayArray = Array.isArray(weekDays) ? weekDays : Object.keys(weekDays).map(Number);
      if (!weekDays || weekDays.length === 0) return '';

      const formattedDays = weekDays
        .sort((a, b) => a - b)  // 确保日期是有序的
        .map(day => weekMap[day]);

      return '星期' + formattedDays.join(' ');
    },
    getTaskTitle(taskItem) {
      switch (taskItem.TimeType) {
        case 0:
          return `${taskItem.PowerOnTime} - ${taskItem.PowerOffTime}`;
        case 1:
          return `${taskItem.PowerOnTime}`;
        case 2:
          return `${taskItem.PowerOffTime}`;
        default:
          return '';
      }
    },
    getTaskSubtitle(taskItem) {
      var typeTitle = '';
      switch (taskItem.TimeType) {
        case 0:
          typeTitle = '开启时段｜';
          break;
        case 1:
          typeTitle = '定时开机｜';
          break;
        case 2:
          typeTitle = '定时关机｜';
          break;
        default:
          return '';
      }
      switch (taskItem.RepeatMode) {
        case 0:
          return typeTitle + '执行一次';
        case 1:
          return typeTitle + '每天';
        case 2:
          return typeTitle + this.getWeekDaysString(taskItem.RepeatWeek);
      }
    },
    getConditionTaskTitle(taskItem) {
      var titleText = '';
      if (taskItem.ConditionType == 0) {
        titleText += '输入';
      }
      else if (taskItem.ConditionType == 1) {
        titleText += '输出';
      }

      if (taskItem.ConditionQuantity == 0) {
        titleText += '电流';
      }
      else if (taskItem.ConditionQuantity == 1) {
        titleText += '功率';
      }

      if (taskItem.CompareType == 0) {
        titleText += ' > ';
      }
      else if (taskItem.CompareType == 1) {
        titleText += ' < ';
      }

      if (taskItem.ConditionQuantity == 0) {
        titleText += taskItem.ConditionValue + 'A';
      }
      else if (taskItem.ConditionQuantity == 1) {
        titleText += taskItem.ConditionValue + 'W';
      }

      titleText += ' ' + taskItem.ConditionMinutes + '分钟';
      return titleText;
    },
  },
  beforeUnmount() {
    clearInterval(this.intervalId); // 组件卸载前清除定时器
    clearInterval(this.countDownInterval);
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

.rotate-animation {
  animation: rotate 1s linear infinite;
}

@keyframes rotate {
  from {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(360deg);
  }
}
</style>
