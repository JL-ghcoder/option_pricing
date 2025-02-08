# CF_coursework - Option Pricing

## 概览
<font style="color:rgb(0, 0, 0);">这是一个 </font>**<font style="color:rgb(0, 0, 0);">Computational Finance Coursework</font>**<font style="color:rgb(0, 0, 0);"> 的实现工程，涵盖从金融数学建模到高性能计算的完整流程。该项目不仅包含了对 </font>**<font style="color:rgb(0, 0, 0);">金融衍生品定价</font>**<font style="color:rgb(0, 0, 0);"> 的深入研究思路和理论分析，还通过 </font>**<font style="color:rgb(0, 0, 0);">C++ 与 Python 的混合编程</font>**<font style="color:rgb(0, 0, 0);"> 实现了一系列高效的数值计算方法，并将其封装为可复用的 Python 库。同时，项目还包含了 </font>**<font style="color:rgb(0, 0, 0);">API 设计</font>**<font style="color:rgb(0, 0, 0);"> 以及 </font>**<font style="color:rgb(0, 0, 0);">数据可视化的 Web 前端</font>**<font style="color:rgb(0, 0, 0);">。</font>

<font style="color:rgb(0, 0, 0);"></font>

该工程实现了：

+ 蒙特卡罗障碍期权定价
+ 障碍期权的敏感性分析
+ payoff概率计算
+ 欧式期权的有限差分定价
+ 欧式期权的二叉树定价
+ 将c++工程打包成高性能python库
+ 封装API并且在网站进行可视化



## <font style="color:rgb(0, 0, 0);">我的思路</font>
### 如何定价
<font style="color:rgb(0, 0, 0);">票据的回报取决于一个标的资产 SS，其支付公式为：</font>

<font style="color:rgb(0, 0, 0);"></font>

$ [S_T-K]^+\cdot1\left(\max_{t\in[0,T]}S_t<B\right) $

<font style="color:rgb(0, 0, 0);">这里：</font>

+ $ S_T $<font style="color:rgb(0, 0, 0);">: 到期时的标的资产价格。</font>
+ $ K $<font style="color:rgb(0, 0, 0);">: 行权价。</font>
+ $ B $<font style="color:rgb(0, 0, 0);">: 障碍水平（若标的资产在整个期间的最高值超过 B，票据变为无效）。</font>
+ $ 1 $<font style="color:rgb(0, 0, 0);">: 指示函数，若事件成立则值为1，否则为0。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">更详细的说：</font>

$ [S_T-K]^+=\max(S_T-K,0) $<font style="color:rgb(0, 0, 0);">是期权支付的一部分，表示 </font>**<font style="color:rgb(0, 0, 0);">标的资产在到期日 T 的价格 ST 超过行权价 K 时的正收益</font>**<font style="color:rgb(0, 0, 0);">。</font>

<font style="color:rgb(0, 0, 0);"></font>

$ 1\left(\max_{t\in[0,T]}S_t<B\right) $<font style="color:rgb(0, 0, 0);">是一个指示函数，</font>**<font style="color:rgb(0, 0, 0);">决定票据是否有效</font>**<font style="color:rgb(0, 0, 0);">。这个B存在的意义一方面是降低投资者的成本，另外一方面也帮助发行方管理风险。</font>

+ <font style="color:rgb(0, 0, 0);">如果在生命周期内 S 的价格曾经超过 B，则票据无效，最终支付为零。</font>
+ <font style="color:rgb(0, 0, 0);">如果价格始终低于 B，票据才可能有正支付。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">我们的目标是使用 </font>**<font style="color:rgb(0, 0, 0);">蒙特卡洛模拟</font>**<font style="color:rgb(0, 0, 0);"> 来估计障碍期权的价格，并且我们已知 </font>**<font style="color:rgb(0, 0, 0);">Payoff</font>**<font style="color:rgb(0, 0, 0);"> 的表达式。</font>



根据$ \text{现值}=\text{未来值}\times e^{-r\cdot t} $（来源于<u><font style="color:rgb(0, 0, 0);">复利计算未来值</font></u>）



我们可以估计期权的价格为：

$ \text{Option Price}=e^{-rT}\cdot\mathbb{E}\left[\mathrm{Payoff}\right] $

<font style="color:rgb(0, 0, 0);">这里</font>$ e^{-rT} $<font style="color:rgb(0, 0, 0);">是折现因子，</font>$ \mathbb{E}\left[\mathrm{Payoff}\right] $<font style="color:rgb(0, 0, 0);">是期望值（通过模拟路径的均值近似）。</font>

<font style="color:rgb(0, 0, 0);">也就是说我们关键点是要尽可能准确的求出</font>$ \mathbb{E}\left[\mathrm{Payoff}\right] $



我们首先要模拟资产价格ST的运动，假设它遵循**<font style="color:rgb(0, 0, 0);">几何布朗运动</font>**

$ S_{t+\Delta t}=S_t\cdot\exp\left(\left(r-\mathrm{div}-\frac{1}{2}\sigma^2\right)\Delta t+\sigma\sqrt{\Delta t}Z\right) $

+ $ r $<font style="color:rgb(0, 0, 0);"> 是无风险利率。</font>
+ $ div $<font style="color:rgb(0, 0, 0);"> 是股息收益率。</font>
+ $ \sigma $<font style="color:rgb(0, 0, 0);"> 是波动率。</font>
+ <font style="color:rgb(0, 0, 0);"></font>$ \Delta t =T / numSteps $<font style="color:rgb(0, 0, 0);">是时间步长。</font>
+ <font style="color:rgb(0, 0, 0);">Z ∼ N(0,1) 是标准正态分布的随机变量。</font>



缩写后：

$ S_{t+\Delta t}=S_t\cdot\exp\left(drift+diffusion\cdot Z\right) $

$ drift=\left(r-\mathrm{div}-\frac{1}{2}\sigma^2\right)\Delta t $

$ diffusion=\sigma\sqrt{\Delta t} $

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">在路径模拟中，初始资产价格为 S0，通过上述公式依次迭代模拟 S1,S2,…,ST。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">在每次路径模拟中，记录路径中的</font>**<font style="color:rgb(0, 0, 0);">最高价格</font>**<font style="color:rgb(0, 0, 0);">：</font>

$ \max_{t\in[0,T]}S_t $

<font style="color:rgb(0, 0, 0);">在每一步t时我们都应该检查当前价格 St 是否为路径中的最大值，更新 max⁡S。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">路径模拟完成后，检查障碍条件：</font>

$ \mathrm{If~}\max_{t\in[0\mathrm{~T}]}S_t<B,\text{then valid path. Otherwise, payoff}=0 $



<font style="color:rgb(0, 0, 0);">模拟完成所有路径后，将支付的总和取均值，并乘以折现因子 </font>$ e^{-rT} $

$ \text{Option Price}=e^{-rT}\cdot\frac{\sum_{i=1}^N\mathrm{Payoff}_i}{N} $

+ <font style="color:rgb(0, 0, 0);">N</font><font style="color:rgb(0, 0, 0);">=</font><font style="color:rgb(0, 0, 0);">numPaths</font><font style="color:rgb(0, 0, 0);"> </font><font style="color:rgb(0, 0, 0);">是模拟路径数。</font>
+ <font style="color:rgb(0, 0, 0);">Payoffi 是第 i 条路径的支付。</font>

<font style="color:rgb(0, 0, 0);"></font>

### <font style="color:rgb(0, 0, 0);">敏感性分析</font>
<font style="color:rgb(0, 0, 0);">我们需要分析 </font>**<font style="color:rgb(0, 0, 0);">波动率 (σ) 的变化</font>**<font style="color:rgb(0, 0, 0);"> 对期权价格的影响。</font>

<font style="color:rgb(0, 0, 0);"></font>

+ <font style="color:rgb(0, 0, 0);">定义波动率的变化范围：从</font>$ \sigma_{\mathrm{start}} $<font style="color:rgb(0, 0, 0);">到</font>$ \sigma_{\mathrm{end}} $<font style="color:rgb(0, 0, 0);">。</font>
+ <font style="color:rgb(0, 0, 0);">将波动率分为若干点：</font>



$ \sigma_i=\sigma_\mathrm{start}+i\cdot\Delta\sigma,\quad i=0,1,\ldots,n-1 $



其中：

+ $ \Delta\sigma=\frac{\sigma_{\mathrm{end}}-\sigma_{\mathrm{start}}}{n-1} $<font style="color:rgb(0, 0, 0);">是波动率的步长。</font>
+ <font style="color:rgb(0, 0, 0);">n: 采样点数。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">对每个波动率</font>$ \sigma_{i} $<font style="color:rgb(0, 0, 0);">，通过蒙特卡洛模拟计算期权价格：</font>

$ P(\sigma_i)=e^{-rT}\cdot\frac{1}{N}\sum_{j=1}^N\mathrm{Payoff}_j(\sigma_i) $

<font style="color:rgb(0, 0, 0);">其中：</font>

+ <font style="color:rgb(0, 0, 0);">N: 每个波动率下的模拟路径数。</font>
+ $ \mathrm{Payoff}_j(\sigma_i) $<font style="color:rgb(0, 0, 0);">: 第 j 条路径的支付值，依赖于当前波动率 </font>$ \sigma_i $<font style="color:rgb(0, 0, 0);">。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">将每组结果 </font>$ (\sigma_i,P(\sigma_i)) $<font style="color:rgb(0, 0, 0);"> 记录下来</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">对于每个波动率 </font>$ \sigma_i $<font style="color:rgb(0, 0, 0);">，标的资产价格路径 </font>$ S_{t} $<font style="color:rgb(0, 0, 0);"> 通过 </font>**<font style="color:rgb(0, 0, 0);">几何布朗运动（GBM）</font>**<font style="color:rgb(0, 0, 0);"> 模型生成：</font>

$ S_{t+\Delta t}=S_t\cdot\exp\left((r-\frac{1}{2}\sigma_i^2)\Delta t+\sigma_i\sqrt{\Delta t}Z\right) $



$ \mathrm{Results}=\{(\sigma_0,P(\sigma_0)),(\sigma_1,P(\sigma_1)),\ldots,(\sigma_{n-1},P(\sigma_{n-1}))\} $



### 概率计算
<font style="color:rgb(0, 0, 0);">这道题目要求我们结合代码中的蒙特卡洛模拟，计算 </font>**<font style="color:rgb(0, 0, 0);">障碍期权的支付值大于 5 美元的概率</font>**<font style="color:rgb(0, 0, 0);">。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);"></font>$ P(\mathrm{Payoff}>5)=\frac{\text{有效路径数量(支付}>5)}{\text{总路径数量}} $

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">这意味着需要模拟许多条路径，逐条判断支付值是否满足条件。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">对于每条路径：</font>

+ <font style="color:rgb(0, 0, 0);">如果路径的最大值 max⁡St≥B，则障碍条件失效，支付值为 0。</font>
+ <font style="color:rgb(0, 0, 0);">如果路径的最大值 max⁡St<B，则计算支付值：</font>$ \mathrm{Payoff}=\max(S_T-K,0) $<font style="color:rgb(0, 0, 0);">。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">然后统计满足 Payoff>5的路径数量，并计算其占总路径数的比例。</font>

<font style="color:rgb(0, 0, 0);"></font>

### <font style="color:rgb(0, 0, 0);">欧式看跌期权的有限差分定价</font>
<font style="color:rgb(0, 0, 0);">欧式看跌期权的定价是基于</font>**<font style="color:rgb(0, 0, 0);">有限差分法（Finite Difference Method, FDM）</font>**<font style="color:rgb(0, 0, 0);">的。</font>

<font style="color:rgb(0, 0, 0);">欧式看跌期权的价值由Black-Scholes偏微分方程（PDE）描述：</font>

<font style="color:rgb(0, 0, 0);"></font>

$ \frac{\partial V}{\partial t}+\frac{1}{2}\sigma^2S^2\frac{\partial^2V}{\partial S^2}+rS\frac{\partial V}{\partial S}-rV=0 $



+ <font style="color:rgb(0, 0, 0);">V(S,t)：期权在价格为 S、时间为 t 时的价值。</font>
+ <font style="color:rgb(0, 0, 0);">σ：标的资产的波动率。</font>
+ <font style="color:rgb(0, 0, 0);">r：无风险利率。</font>
+ <font style="color:rgb(0, 0, 0);">S：标的资产价格。</font>
+ <font style="color:rgb(0, 0, 0);">t：时间（从当前时间到到期时间的倒计时）。</font>

**<font style="color:rgb(0, 0, 0);"></font>**

**<font style="color:rgb(0, 0, 0);">边界条件</font>**<font style="color:rgb(0, 0, 0);">：</font>

**<font style="color:rgb(0, 0, 0);">终端条件（Terminal Condition）</font>**<font style="color:rgb(0, 0, 0);">：</font>

$ V(S,T)=\max(K-S,0) $<font style="color:rgb(0, 0, 0);">（到期时的支付）</font>

<font style="color:rgb(0, 0, 0);"></font>

**<font style="color:rgb(0, 0, 0);">边界条件（Boundary Conditions）</font>**<font style="color:rgb(0, 0, 0);">：</font>

<font style="color:rgb(0, 0, 0);">当 S→0 时：</font>

$ V(0,t)=Ke^{-r(T-t)} $<font style="color:rgb(0, 0, 0);"> （到期时期权价值等于行权价的现值）</font>

<font style="color:rgb(0, 0, 0);">当 S→∞ 时：</font>

$ V(S,t)\to0 $<font style="color:rgb(0, 0, 0);"> （标的价格极高时，看跌期权无价值）</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">有限差分法通过将上述 PDE 离散化，在网格上近似求解。</font>

<font style="color:rgb(0, 0, 0);"></font>

**<font style="color:rgb(0, 0, 0);">网格定义</font>**<font style="color:rgb(0, 0, 0);">：</font>

**<font style="color:rgb(0, 0, 0);">资产价格网格</font>**<font style="color:rgb(0, 0, 0);">： 将资产价格 S 离散为等距点：</font>

$ S_i=i\cdot\Delta S,\quad i=0,1,\ldots,N $

其中$ \Delta S=\frac{S_{\max}}{N} $，$ S_{\mathrm{max}} $<font style="color:rgb(0, 0, 0);">是价格的上界（这里设置为 2K）。</font>

**<font style="color:rgb(0, 0, 0);">时间网格</font>**<font style="color:rgb(0, 0, 0);">： 将时间 T 离散为 M 个时间步：</font>

$ t_j=j\cdot\Delta t,\quad j=0,1,\ldots,M $

<font style="color:rgb(0, 0, 0);">其中</font>$ \Delta t=\frac{T}{M} $<font style="color:rgb(0, 0, 0);">。</font>

<font style="color:rgb(0, 0, 0);">需要注意的是：在显式有限差分方法中，时间步长 Δt 必须满足如下稳定性条件（由 Courant-Friedrichs-Lewy 条件推导）：</font>

$ \Delta t\leq\frac{\Delta S^2}{\sigma^2S_{\max}^2} $

$ \Delta t\leq\frac{1}{\sigma^2N^2} $

<font style="color:rgb(0, 0, 0);"></font>

**<font style="color:rgb(0, 0, 0);">离散化偏导数</font>**<font style="color:rgb(0, 0, 0);">：</font>

**<font style="color:rgb(0, 0, 0);">一阶导数（Delta）</font>**<font style="color:rgb(0, 0, 0);">：</font>

$ \frac{\partial V}{\partial S}\approx\frac{V_{i+1}-V_{i-1}}{2\Delta S} $



**<font style="color:rgb(0, 0, 0);">二阶导数（Gamma）</font>**<font style="color:rgb(0, 0, 0);">：</font>

$ \frac{\partial^2V}{\partial S^2}\approx\frac{V_{i+1}-2V_i+V_{i-1}}{\Delta S^2} $



**<font style="color:rgb(0, 0, 0);">时间导数</font>**<font style="color:rgb(0, 0, 0);">：</font>

$ \frac{\partial V}{\partial t}\approx\frac{V_i^{j+1}-V_i^j}{\Delta t} $



**<font style="color:rgb(0, 0, 0);">离散化后的 PDE</font>**<font style="color:rgb(0, 0, 0);">： 将 PDE 转换为有限差分公式（显式方法）：</font>

$ V_i^j=V_i^{j+1}+\Delta t\left(0.5\sigma^2S_i^2\frac{V_{i+1}-2V_i+V_{i-1}}{\Delta S^2}+rS_i\frac{V_{i+1}-V_{i-1}}{2\Delta S}-rV_i\right) $

$ \Gamma\approx\frac{V(S_{i+1},t)-2V(S_i,t)+V(S_{i-1},t)}{\Delta S^2} $

$ \Delta\approx\frac{V(S_{i+1},t)-V(S_{i-1},t)}{2\Delta S} $

<font style="color:rgb(0, 0, 0);">其中：</font>

$ V_i^{j} $<font style="color:rgb(0, 0, 0);">代表时间 tj 时资产价格为 Si 的期权价值。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">插值公式的目的是在离散网格上找到给定资产价格 spot的期权价值。</font>

<font style="color:rgb(0, 0, 0);">在离散网格 Si 和对应的期权价值 V(Si,t) 中，给定 spot，其不一定在 Si 上。我们需要使用插值法估算 V(spot,t)。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">设 spot 落在 [Si,Si+1] 之间，即：</font>

$ S_i\leq\mathrm{spot}\leq S_{i+1} $



<font style="color:rgb(0, 0, 0);">线性插值公式为：</font>

$ V(spot,t)\approx V(S_i,t)+\frac{spot-S_i}{S_{i+1}-S_i}\cdot(V(S_{i+1},t)-V(S_i,t)) $

+ <font style="color:rgb(0, 0, 0);">V(Si,t)：网格点 Si 处的期权价值。</font>
+ <font style="color:rgb(0, 0, 0);">V(Si+1,t)：网格点 Si+1 处的期权价值。</font>
+ $ \frac{\mathrm{spot}-S_i}{S_{i+1}-S_i} $<font style="color:rgb(0, 0, 0);">：spot 在 Si 和 Si+1 之间的比例。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">在代码结构上的改进：</font>

+ <font style="color:rgb(0, 0, 0);">使用vector替代原始指针，避免内存泄漏</font>
+ <font style="color:rgb(0, 0, 0);">添加了辅助函数进行价格插值</font>



+ <font style="color:rgb(0, 0, 0);">正确实现了欧式看跌期权的终值条件 </font>`<font style="color:rgb(0, 0, 0);">max(strike - S[i], 0.0)</font>`
+ <font style="color:rgb(0, 0, 0);">使用更合理的价格上界（2倍执行价格）</font>
+ <font style="color:rgb(0, 0, 0);">添加了适当的边界条件</font>
+ <font style="color:rgb(0, 0, 0);">改进了网格参数的设置（dt有要求-CFL条件）</font>
+ <font style="color:rgb(0, 0, 0);">添加了线性插值方法以获得精确spot价格对应的期权价值</font>



### 欧式看涨期权的<font style="color:rgb(0, 0, 0);">二叉树定价</font>
<font style="color:rgb(0, 0, 0);">二叉树法将期权价格建模为标的资产价格的离散过程。在每个时间步，标的资产价格要么上升要么下降，形成一个二叉树结构。</font>

<font style="color:rgb(0, 0, 0);">期权的价值通过 </font>**<font style="color:rgb(0, 0, 0);">向后归纳法</font>**<font style="color:rgb(0, 0, 0);"> 计算，从到期日开始向当前时间倒推，使用 </font>**<font style="color:rgb(0, 0, 0);">风险中性概率</font>**<font style="color:rgb(0, 0, 0);"> 计算期权的期望价值。</font>

<font style="color:rgb(0, 0, 0);"></font>

根据BS公式：

<font style="color:rgb(0, 0, 0);">标的资产价格 St在连续时间下的随机过程由以下公式描述（Black-Scholes 模型假设）：</font>

$ dS_t=rS_t\left.dt+\sigma S_t\right.dW_t $

+ <font style="color:rgb(0, 0, 0);">r</font><font style="color:rgb(0, 0, 0);">：无风险利率。</font>
+ <font style="color:rgb(0, 0, 0);">σ：标的资产的波动率。</font>
+ <font style="color:rgb(0, 0, 0);">dWt：标准布朗运动的增量。</font>
+ <font style="color:rgb(0, 0, 0);">St：资产价格。</font>



<font style="color:rgb(0, 0, 0);">解这个随机微分方程的结果是：</font>

$ S_t=S_0\cdot e^{\left(r-\frac{1}{2}\sigma^2\right)t+\sigma W_t} $



<font style="color:rgb(0, 0, 0);">为了通过二叉树法模拟资产价格，我们将时间离散化为 n 个步长，每步的时间间隔为 Δt=T/n。在每个时间步，资产价格要么上涨，要么下跌。</font>

**<font style="color:rgb(0, 0, 0);">资产价格的变动率 (Δξ)</font>**

+ <font style="color:rgb(0, 0, 0);">假设每个时间步资产价格的对数变化是常量 Δξ。</font>
+ <font style="color:rgb(0, 0, 0);">在每个时间步 j：</font>
    - <font style="color:rgb(0, 0, 0);">如果资产价格上涨，变动为 +Δξ。</font>
    - <font style="color:rgb(0, 0, 0);">如果资产价格下跌，变动为 −Δξ。</font>

<font style="color:rgb(0, 0, 0);"></font>

**<font style="color:rgb(0, 0, 0);">资产价格的离散表达</font>**<font style="color:rgb(0, 0, 0);"> 在二叉树模型中，第 j 个时间步的第 i 个节点表示资产价格经历了 i 次上涨和 j−i 次下跌。因此：</font>

$ S_i^j=S_0\cdot e^{i\cdot\Delta\xi-(j-i)\cdot\Delta\xi}=S_0\cdot e^{(2i-j)\cdot\Delta\xi} $



为什么我们可以得到：$ \Delta\xi=\sqrt{\sigma^2\Delta t+\nu^2\Delta t^2} $

<font style="color:rgb(0, 0, 0);">漂移项 v 定义为：</font>

<font style="color:rgb(0, 0, 0);"></font>$ \nu=r-0.5\sigma^2 $

+ <font style="color:rgb(0, 0, 0);">r：无风险利率，表示资产价格的长期增长趋势。</font>
+ $ -0.5\sigma^2 $<font style="color:rgb(0, 0, 0);">：波动率带来的减项，修正资产价格在风险中性条件下的增长率。</font>

<font style="color:rgb(0, 0, 0);">在每个时间步，资产价格受到漂移项的影响，价格变化为 νΔt。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">而波动率项 (σΔt)</font>

<font style="color:rgb(0, 0, 0);">波动率项来源于布朗运动的随机性：</font>

$ \Delta S_t=\sigma S_tZ\sqrt{\Delta t},\quad Z\sim\mathcal{N}(0,1) $

<font style="color:rgb(0, 0, 0);">每个时间步的波动率引入了随机变化，变化幅度为 </font>$ \sigma\sqrt{\Delta t} $<font style="color:rgb(0, 0, 0);">。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">将漂移项和波动率项结合，得到资产价格在每个时间步的对数变化：</font>

$ \Delta\ln S\approx\nu\Delta t\pm\sigma\sqrt{\Delta t} $

<font style="color:rgb(0, 0, 0);">为了更一般化地表示资产价格的上升或下降，定义一个统一的变化量 Δξ，它同时包含漂移项和波动率项：</font>

$ \Delta\xi=\sqrt{\sigma^2\Delta t+\nu^2\Delta t^2} $



为什么：$ S_i^j=S_0\cdot e^{(2i-j)\cdot\Delta\xi} $

<font style="color:rgb(0, 0, 0);">在每个时间步 j：</font>

+ <font style="color:rgb(0, 0, 0);">资产价格在第 i 个节点处表示经历了 i 次上涨和 j−i 次下跌。</font>
+ <font style="color:rgb(0, 0, 0);">每次上涨对应 +Δξ，每次下跌对应 −Δξ。</font>

<font style="color:rgb(0, 0, 0);">因此，总的对数变化为：</font>

$ \ln S_i^j=\ln S_0+i\cdot\Delta\xi-(j-i)\cdot\Delta\xi $

$ \ln S_i^j=\ln S_0+(2i-j)\cdot\Delta\xi $

$ S_i^j=S_0\cdot e^{(2i-j)\cdot\Delta\xi} $

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);"></font>

**<font style="color:rgb(0, 0, 0);">树的参数定义</font>**

**<font style="color:rgb(0, 0, 0);">时间步长 (Δt)</font>**<font style="color:rgb(0, 0, 0);">：</font>

$ \Delta t=\frac{T}{n} $

<font style="color:rgb(0, 0, 0);">T：期权的到期时间。</font>

<font style="color:rgb(0, 0, 0);">n：时间步的总数。</font>



**<font style="color:rgb(0, 0, 0);">资产价格变动幅度 (Δξ)</font>**<font style="color:rgb(0, 0, 0);">： 在每个时间步，资产价格变化由波动率和漂移项决定：</font>

<font style="color:rgb(0, 0, 0);"></font>$ \Delta\xi=\sqrt{\sigma^2\Delta t+\nu^2\Delta t^2} $

<font style="color:rgb(0, 0, 0);">其中：</font>

+ <font style="color:rgb(0, 0, 0);">σ：波动率（volatility）。</font>
+ $ \nu=r-0.5\sigma^2 $<font style="color:rgb(0, 0, 0);">：漂移项，反映风险中性世界下资产价格的趋势。</font>

<font style="color:rgb(0, 0, 0);"></font>

**<font style="color:rgb(0, 0, 0);">风险中性概率 (p)</font>**<font style="color:rgb(0, 0, 0);">： 资产价格上升的概率：</font>

<font style="color:rgb(0, 0, 0);"></font>$ p=0.5+0.5\cdot\frac{\nu\Delta t}{\Delta\xi} $

<font style="color:rgb(0, 0, 0);">风险中性概率是无套利条件下使用贴现期望计算期权价值的核心。</font>

<font style="color:rgb(0, 0, 0);"></font>

**<font style="color:rgb(0, 0, 0);">构建资产价格树</font>**

<font style="color:rgb(0, 0, 0);">资产价格的计算公式为：</font>

<font style="color:rgb(0, 0, 0);"></font>$ S_i^j=S_0\cdot e^{(2i-j)\cdot\Delta\xi} $

+ <font style="color:rgb(0, 0, 0);">Sij：第 j 个时间步的第 i 个节点的资产价格。</font>
+ <font style="color:rgb(0, 0, 0);">S0：初始资产价格。</font>
+ <font style="color:rgb(0, 0, 0);">Δξ：每步资产价格的变动幅度。</font>

**<font style="color:rgb(0, 0, 0);"></font>**

**<font style="color:rgb(0, 0, 0);">计算到期时的期权价值</font>**

<font style="color:rgb(0, 0, 0);">到期时（</font><font style="color:rgb(0, 0, 0);">t</font><font style="color:rgb(0, 0, 0);">=</font><font style="color:rgb(0, 0, 0);">T</font><font style="color:rgb(0, 0, 0);">t</font><font style="color:rgb(0, 0, 0);">=</font><font style="color:rgb(0, 0, 0);">T</font><font style="color:rgb(0, 0, 0);">），根据期权类型计算支付值：</font>

1. **<font style="color:rgb(0, 0, 0);">看涨期权（Call Option）</font>**<font style="color:rgb(0, 0, 0);">：</font>$ V_i^n=\max(S_i^n-K,0) $
2. **<font style="color:rgb(0, 0, 0);">看跌期权（Put Option）</font>**<font style="color:rgb(0, 0, 0);">：</font>$ V_i^n=\max(K-S_i^n,0) $
    - <font style="color:rgb(0, 0, 0);">Sin：到期时第 i 个节点的资产价格。</font>
    - <font style="color:rgb(0, 0, 0);">K：行权价。</font>

<font style="color:rgb(0, 0, 0);"></font>

<font style="color:rgb(0, 0, 0);">向后归纳计算期权价值</font>

<font style="color:rgb(0, 0, 0);">在 j 时刻的第 i 个节点，期权价值通过下一时间步的期权价值的期望值计算：</font>

$ V_i^j=e^{-r\Delta t}\cdot\left(p\cdot V_{i+1}^{j+1}+(1-p)\cdot V_i^{j+1}\right) $



<font style="color:rgb(0, 0, 0);">使用vector替代原始指针</font>

<font style="color:rgb(0, 0, 0);">修复了时间步长dt的计算</font>

<font style="color:rgb(0, 0, 0);">修正了风险中性概率p的计算</font>

<font style="color:rgb(0, 0, 0);">改进了价格树的构建方式</font>



## <font style="color:rgb(0, 0, 0);">项目结构设计</font>
这样设计比较合理：

```cpp
CF_COURSEWORK/
├── analyze/               # 数据分析模块
│   └── ...                
├── build/                 # C++ 构建生成的文件目录
│   └── ...                
├── data/                  # 数据目录
│   └── ...               
├── external/              # 外部依赖或库
│   └── pybind11                
├── include/               # C++ 头文件目录
│   └── ...                
├── src/                   # 核心算法和实现代码
│   └── ...                
├── test/                  # 测试代码目录
│   └── ...                
├── web_visualization/     # 前端可视化模块
│   └── ...               
├── .gitignore            
├── app.py                 # API 服务
├── binding.cpp            # Python 和 C++ 的绑定代码
├── CMakeLists.txt         
├── option_pricing.cpython-310-*.so  # C++ 打包生成的高性能 Python 库
└── README.md              

```



通过pybind11我们可以把这个C++程序打包成python的module

然后接下来的可视化与分析工作就全部移到python中更方便的进行



启动fastapi

```cpp
uvicorn app:app --reload
```



## C++与python接口
```cpp
class MonteCarloBarrierOption {
public:
    static double mainPricer(
        double S0,        // 初始价格
        double K,         // 执行价格
        double B,         // 障碍价格
        double T,         // 到期时间
        double r,         // 无风险利率
        double sigma,     // 波动率
        double div,       // 股息率
        int numSteps = 252,   // 时间步数
        int numPaths = 10000  // 路径数
    );
};

# python

py::class_<MonteCarloBarrierOption>(m, "MonteCarloBarrierOption")
    .def_static("price", &MonteCarloBarrierOption::mainPricer);
```



```cpp
class SensitivityTest {
// numPoints决定sigma的步长，numPaths决定路径数
public:
    // 这个方法用于接口
    static std::vector<std::pair<double, double>> sensitivityAnalysis(
        double S0, double K, double B, double T, double r, double div = 0.0,
        double volStart = 0.1, double volEnd = 0.4, int numPoints = 10,
        int numPaths = 10000);
    
    static std::vector<std::pair<double, double>> sensitivityAnalysisWithCSV(
        double S0, double K, double B, double T, double r, double div = 0.0,
        const std::string& csvPath = "sensitivity_results.csv",
        double volStart = 0.1, double volEnd = 0.4, 
        int numPoints = 10, int numPaths = 10000);
};

#endif

# python

py::class_<SensitivityTest>(m, "SensitivityTest")
    .def_static("analyze", &SensitivityTest::sensitivityAnalysis);
```



```cpp
class PayoffProbability {
public:
    static double calculateProbability(
        double S0,        // Initial price
        double K,         // Strike price
        double B,         // Barrier price
        double T,         // Time to maturity
        double r,         // Risk-free rate
        double sigma,     // Volatility
        double div,       // Dividend yield
        double threshold, // Payoff threshold
        int numPaths = 100000
    );
};
# python

py::class_<PayoffProbability>(m, "PayoffProbability")
    .def_static("calculate", &PayoffProbability::calculateProbability);
```



```cpp
class FiniteDifferencePricer {
private:
    // 插值法
    static double interpolatePrice(const std::vector<double>& grid, 
                          const std::vector<double>& values, 
                          double spot);
    static double maxi(double a, double b);

public:
    // Main pricing function for European put option
    static double priceEuroPutOption(double spot, double strike, 
                             double r, double vol, double T, double div = 0,
                             int gridSize = 100);

    static double priceEuroCallOption(double spot, double strike, 
                             double r, double vol, double T, double div = 0,
                             int gridSize = 100);
};

# python

py::class_<FiniteDifferencePricer>(m, "FiniteDifferencePricer")
    .def_static("price_put", &FiniteDifferencePricer::priceEuroPutOption)
    .def_static("price_call", &FiniteDifferencePricer::priceEuroCallOption);
```



```cpp
class BinomialPricer {
private:
    static double maxi(double a, double b);

public:
    // 欧式看涨期权定价
    static double priceEuroCall(double S0, double K, double r, 
                              double vol, double T, int steps);
    
    // 欧式看跌期权定价
    static double priceEuroPut(double S0, double K, double r, 
                             double vol, double T, int steps);
                             
    // 通用二叉树定价方法，isCall参数决定是看涨还是看跌
    static double btPricer(double S0, double K, double r, double vol,
                          double T, int steps, bool isCall);
};

# python

py::class_<BinomialPricer>(m, "BinomialPricer")
    .def_static("price_call", &BinomialPricer::priceEuroCall)
    .def_static("price_put", &BinomialPricer::priceEuroPut);
```



## API端点
```cpp
/price/barrier：蒙特卡洛障碍期权定价
/price/european：欧式期权定价（同时返回有限差分和二叉树方法的结果）
/analysis/sensitivity：波动率敏感性分析
/analysis/probability：收益概率分析
/generate/paths：价格路径生成
/defaults：默认参数获取
```



```cpp
# 障碍期权定价测试
curl -X POST "http://localhost:8000/price/barrier" \
  -H "Content-Type: application/json" \
  -d '{
    "S0": 100.0,
    "K": 100.0,
    "B": 120.0,
    "T": 1.0,
    "r": 0.05,
    "sigma": 0.2,
    "div": 0.0,
    "num_steps": 252,
    "num_paths": 100000
  }'

# 有限差分法看跌期权定价测试
curl -X POST "http://localhost:8000/price/finite-difference/put" \
  -H "Content-Type: application/json" \
  -d '{
    "S0": 100.0,
    "K": 100.0,
    "T": 1.0,
    "r": 0.05,
    "sigma": 0.2,
    "div": 0.0,
    "grid_size": 100
  }'

# 有限差分法看涨期权定价测试
curl -X POST "http://localhost:8000/price/finite-difference/call" \
  -H "Content-Type: application/json" \
  -d '{
    "S0": 100.0,
    "K": 100.0,
    "T": 1.0,
    "r": 0.05,
    "sigma": 0.2,
    "div": 0.0,
    "grid_size": 100
  }'

# 敏感性分析测试
curl -X POST "http://localhost:8000/analysis/sensitivity" \
  -H "Content-Type: application/json" \
  -d '{
    "S0": 100.0,
    "K": 100.0,
    "B": 120.0,
    "T": 1.0,
    "r": 0.05,
    "div": 0.0,
    "vol_start": 0.1,
    "vol_end": 0.4,
    "num_paths": 5,
    "num_points": 5
  }'

# 收益概率计算测试
curl -X POST "http://localhost:8000/analysis/probability" \
  -H "Content-Type: application/json" \
  -d '{
    "S0": 100.0,
    "K": 100.0,
    "B": 120.0,
    "T": 1.0,
    "r": 0.05,
    "sigma": 0.2,
    "div": 0.0,
    "threshold": 5.0,
    "num_paths": 100000
  }'
```

